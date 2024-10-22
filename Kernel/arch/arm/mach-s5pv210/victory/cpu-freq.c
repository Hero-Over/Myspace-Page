/*
 *  linux/arch/arm/plat-s5pc11x/s5pc11x-cpufreq.c
 *
 *  CPU frequency scaling for S5PC110
 *
 *  Copyright (C) 2008 Samsung Electronics
 *
 *  Based on cpu-sa1110.c, Copyright (C) 2001 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <asm/system.h>

#include <mach/hardware.h>
#include <mach/map.h>

#include <mach/regs-clock.h>
#include <mach/cpu-freq-v210.h>
#include <plat/pll.h>
#include <plat/clock.h>
#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#include <linux/earlysuspend.h>
#include <linux/suspend.h>
#endif

#define ENABLE_DVFS_LOCK_HIGH 1
#define USE_DVS
#define GPIO_BASED_DVS

#define DBG(fmt...)
//#define DBG(fmt...) printk(fmt)


unsigned int dvfs_change_direction;
#define CLIP_LEVEL(a, b) (a > b ? b : a)

unsigned int MAXFREQ_LEVEL_SUPPORTED = 6;
unsigned int S5PC11X_MAXFREQLEVEL = 6;
unsigned int S5PC11X_FREQ_TAB;
static spinlock_t g_dvfslock = SPIN_LOCK_UNLOCKED;
static unsigned int s5pc11x_cpufreq_level = 3;
unsigned int s5pc11x_cpufreq_index = 0;

static char cpufreq_governor_name[CPUFREQ_NAME_LEN] = "conservative";// default governor
static char userspace_governor[CPUFREQ_NAME_LEN] = "userspace";
static char conservative_governor[CPUFREQ_NAME_LEN] = "conservative";
int s5pc11x_clk_dsys_psys_change(int index);
int s5pc11x_armclk_set_rate(struct clk *clk, unsigned long rate);

unsigned int prevIndex = 0;

static struct clk * mpu_clk;
#ifdef CONFIG_CPU_FREQ_LOG
static void inform_dvfs_clock_status(struct work_struct *work);
static DECLARE_DELAYED_WORK(dvfs_info_print_work, inform_dvfs_clock_status);
#endif
#if ENABLE_DVFS_LOCK_HIGH
unsigned int g_dvfs_high_lock_token = 0;
static DEFINE_MUTEX(dvfs_high_lock);
unsigned int g_dvfs_high_lock_limit = 4;
unsigned int g_dvfslockval[NUMBER_OF_LOCKTOKEN];
bool g_dvfs_fix_lock_limit = false; // global variable to avoid up frequency scaling 

#endif //ENABLE_DVFS_LOCK_HIGH

extern int store_up_down_threshold(unsigned int down_threshold_value,
				unsigned int up_threshold_value);

/* frequency */
static struct cpufreq_frequency_table s5pc110_freq_table_1GHZ[] = {
	{L0, 1280*1000}, //added 1200 step to table to match 1d2GHZ table below
	{L1, 1000*1000},
	{L2, 800*1000},
	{L3, 400*1000},
	{L4, 200*1000},
	{L5, 100*1000},
	{0, CPUFREQ_TABLE_END},
};

/*Assigning different index for fast scaling up*/
static unsigned char transition_state_1GHZ[][2] = {
        {1, 0}, //added index to table to match 1d2GHZ table below
        {2, 0},
        {3, 1},
        {4, 2},
        {5, 3},
        {5, 4},
};

/* frequency */
static struct cpufreq_frequency_table s5pc110_freq_table_1d2GHZ[] = {
	{L0, 1200*1000},
	{L1, 1000*1000},
	{L2, 800*1000},
	{L3, 400*1000},
	{L4, 200*1000},
	{L5, 100*1000},
	{0, CPUFREQ_TABLE_END},
};

/*Assigning different index for fast scaling up*/
static unsigned char transition_state_1d2GHZ[][2] = {
        {1, 0},
        {2, 0},
        {3, 1},
        {4, 2},
        {5, 3},
        {5, 4},        
};


static unsigned char (*transition_state[2])[2] = {
        transition_state_1GHZ,
        transition_state_1d2GHZ,
};

static struct cpufreq_frequency_table *s5pc110_freq_table[] = {
        s5pc110_freq_table_1GHZ,
        s5pc110_freq_table_1d2GHZ,
};

static unsigned int s5pc110_thres_table_1GHZ[][2] = {
      	{40, 80}, //added this entry (best guess until i figure out what exactly this is)
      	{40, 90},
        {40, 90},
        {40, 80},
        {40, 70},
        {30, 70},
};

static unsigned int s5pc110_thres_table_1d2GHZ[][2] = {
      	{30, 70},
        {30, 70},
        {30, 70},
        {30, 70},
        {30, 70},
        {30, 70},
};

static unsigned int  (*s5pc110_thres_table[2])[2] = {
	s5pc110_thres_table_1GHZ,
	s5pc110_thres_table_1d2GHZ,
};

/*return performance level */
static int get_dvfs_perf_level(enum freq_level_states freq_level, unsigned int *perf_level)
{
	unsigned int freq=0, index = 0;
	struct cpufreq_frequency_table *freq_tab = s5pc110_freq_table[S5PC11X_FREQ_TAB];
	switch(freq_level)
	{
	case LEV_1000MHZ:
		freq = 1000 * 1000;
		break;
	case LEV_800MHZ:
		freq = 800 * 1000;
		break;
	case LEV_400MHZ:
		freq = 400 * 1000;
		break;
	case LEV_200MHZ:
		freq = 200 * 1000;
		break;
	case LEV_100MHZ:
		freq = 100 * 1000;
		break;
	default:
		printk(KERN_ERR "Invalid freq level\n");
		return -EINVAL;
	}
	while((freq_tab[index].frequency != CPUFREQ_TABLE_END) &&
		(freq_tab[index].frequency != freq)) {
		index++;
	}
	if(freq_tab[index].frequency == CPUFREQ_TABLE_END)
	{
		printk(KERN_ERR "Invalid freq table\n");
		return -EINVAL;	
	}

	*perf_level = freq_tab[index].index;
	return 0;
}


// for active high with event from TS and key
static int dvfs_perf_lock = 0;
int dvfs_change_quick = 0;

// jump to the given performance level
static void set_dvfs_perf_level(unsigned int perf_level) 
{
	unsigned long irqflags;

	spin_lock_irqsave(&g_dvfslock,irqflags);
	if(s5pc11x_cpufreq_index > perf_level) {
		s5pc11x_cpufreq_index = perf_level; // jump to specified level 
		dvfs_change_quick = 1;
	}
	spin_unlock_irqrestore(&g_dvfslock,irqflags);
	return;
}

//Jump to the given frequency level 
void set_dvfs_target_level(enum freq_level_states freq_level) 
{
	unsigned int ret = 0, perf_level=0;
	
	ret = get_dvfs_perf_level(freq_level, &perf_level);
	if(ret)
		return;

	set_dvfs_perf_level(perf_level);
	return;
}

#if ENABLE_DVFS_LOCK_HIGH
//Lock and jump to the given frequency level 
void s5pc110_lock_dvfs_high_level(unsigned int nToken, enum freq_level_states freq_level) 
{
	unsigned int nLevel, ret, perf_level=0;
	//printk("dvfs lock with token %d\n",nToken);
	ret = get_dvfs_perf_level(freq_level, &perf_level);
	if(ret)
		return;
	nLevel = perf_level;	
	if (nToken == DVFS_LOCK_TOKEN_6 ) nLevel--; // token for launcher , this can use 1GHz
	// check lock corruption
	if (g_dvfs_high_lock_token & (1 << nToken) ) printk ("\n\n[DVFSLOCK] lock token %d is already used!\n\n", nToken);
	//mutex_lock(&dvfs_high_lock);
	g_dvfs_high_lock_token |= (1 << nToken);
	g_dvfslockval[nToken] = nLevel;
	if (nLevel <  g_dvfs_high_lock_limit)
		g_dvfs_high_lock_limit = nLevel;
	//mutex_unlock(&dvfs_high_lock);
	set_dvfs_perf_level(nLevel);
}
EXPORT_SYMBOL(s5pc110_lock_dvfs_high_level);

void s5pc110_unlock_dvfs_high_level(unsigned int nToken) 
{
	unsigned int i;
	//printk("dvfs unlock with token %d\n",nToken);
	//mutex_lock(&dvfs_high_lock);
	g_dvfs_high_lock_token &= ~(1 << nToken);
	g_dvfslockval[nToken] = MAXFREQ_LEVEL_SUPPORTED-1;
	g_dvfs_high_lock_limit = MAXFREQ_LEVEL_SUPPORTED-1;

	if (g_dvfs_high_lock_token) {
		for (i=0;i<NUMBER_OF_LOCKTOKEN;i++) {
			if (g_dvfslockval[i] < g_dvfs_high_lock_limit)  g_dvfs_high_lock_limit = g_dvfslockval[i];
		}
	}

	//mutex_unlock(&dvfs_high_lock);
}
EXPORT_SYMBOL(s5pc110_unlock_dvfs_high_level);
#endif //ENABLE_DVFS_LOCK_HIGH

unsigned int s5pc11x_target_frq(unsigned int pred_freq, 
				int flag)
{
	int index;
	//unsigned long irqflags;
	unsigned int freq;

	struct cpufreq_frequency_table *freq_tab = s5pc110_freq_table[S5PC11X_FREQ_TAB];
	
	spin_lock(&g_dvfslock);
	if(freq_tab[0].frequency < pred_freq) {
	   index = 0;	
	   goto s5pc11x_target_frq_end;
	}

	if((flag != 1)&&(flag != -1)) {
		printk("s5pc1xx_target_frq: flag error!!!!!!!!!!!!!");
	}

	index = s5pc11x_cpufreq_index;

	if(freq_tab[index].frequency == pred_freq) {	
		if(flag == 1)
			index = transition_state[S5PC11X_FREQ_TAB][index][1];
		else
			index = transition_state[S5PC11X_FREQ_TAB][index][0];
	}
	/*else {
		index = 0; 
	}*/

	if (g_dvfs_high_lock_token) {
		 if(g_dvfs_fix_lock_limit == true) {
			 index = g_dvfs_high_lock_limit;// use the same level
		 }
		 else {
			if (index > g_dvfs_high_lock_limit)
				index = g_dvfs_high_lock_limit;
		 }
	}
	//printk("s5pc11x_target_frq index = %d\n",index);

s5pc11x_target_frq_end:
	//spin_lock_irqsave(&g_cpufreq_lock, irqflags);
	index = CLIP_LEVEL(index, s5pc11x_cpufreq_level);
	s5pc11x_cpufreq_index = index;
	//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
	
	freq = freq_tab[index].frequency;
	spin_unlock(&g_dvfslock);
	return freq;
}


int s5pc11x_target_freq_index(unsigned int freq)
{
	int index = 0;
	//unsigned long irqflags;
	
	struct cpufreq_frequency_table *freq_tab = s5pc110_freq_table[S5PC11X_FREQ_TAB];

	if(freq >= freq_tab[index].frequency) {
		goto s5pc11x_target_freq_index_end;
	}

	/*Index might have been calculated before calling this function.
	check and early return if it is already calculated*/
	if(freq_tab[s5pc11x_cpufreq_index].frequency == freq) {		
		return s5pc11x_cpufreq_index;
	}

	while((freq < freq_tab[index].frequency) &&
			(freq_tab[index].frequency != CPUFREQ_TABLE_END)) {
		index++;
	}

	if(index > 0) {
		if(freq != freq_tab[index].frequency) {
			index--;
		}
	}

	if(freq_tab[index].frequency == CPUFREQ_TABLE_END) {
		index--;
	}

s5pc11x_target_freq_index_end:
	spin_lock(&g_dvfslock);
	index = CLIP_LEVEL(index, s5pc11x_cpufreq_level);
	s5pc11x_cpufreq_index = index;
	spin_unlock(&g_dvfslock);
	
	return index;
} 


int s5pc110_pm_target(unsigned int target_freq)
{
        int ret = 0;
        unsigned long arm_clk;
        unsigned int index;


        index = s5pc11x_target_freq_index(target_freq);
        if(index == INDX_ERROR) {
           printk("s5pc110_target: INDX_ERROR \n");
           return -EINVAL;
        }

        arm_clk = s5pc110_freq_table[S5PC11X_FREQ_TAB][index].frequency;
        
        target_freq = arm_clk;

#ifdef USE_DVS
#ifdef GPIO_BASED_DVS
	set_voltage_dvs(index);
#else
        set_voltage(index);
#endif
#endif
        /* frequency scaling */
        ret = clk_set_rate(mpu_clk, target_freq * KHZ_T);
        if(ret != 0) {
                printk("frequency scaling error\n");
                return -EINVAL;
        }
#if 1
	/*change the frequency threshold level*/
	store_up_down_threshold(s5pc110_thres_table[S5PC11X_FREQ_TAB][index][0], 
				s5pc110_thres_table[S5PC11X_FREQ_TAB][index][1]);
#endif
        return ret;
}

int is_userspace_gov(void)
{
        int ret = 0;
        //unsigned long irqflags;
        //spin_lock_irqsave(&g_cpufreq_lock, irqflags);
        if(!strnicmp(cpufreq_governor_name, userspace_governor, CPUFREQ_NAME_LEN)) {
                ret = 1;
        }
       // spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
        return ret;
}

int is_conservative_gov(void)
{
        int ret = 0;
        //unsigned long irqflags;
        //spin_lock_irqsave(&g_cpufreq_lock, irqflags);
        if(!strnicmp(cpufreq_governor_name, conservative_governor, CPUFREQ_NAME_LEN)) {
                ret = 1;
        }
       // spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
        return ret;
}


/* TODO: Add support for SDRAM timing changes */

int s5pc110_verify_speed(struct cpufreq_policy *policy)
{

	if (policy->cpu)
		return -EINVAL;

	return cpufreq_frequency_table_verify(policy, s5pc110_freq_table[S5PC11X_FREQ_TAB]);
}

unsigned int s5pc110_getspeed(unsigned int cpu)
{
	unsigned long rate;

	if (cpu)
		return 0;

	rate = clk_get_rate(mpu_clk) / KHZ_T;

	return rate;
}

extern void print_clocks(void);
extern void dvs_set_for_1dot2Ghz (int onoff);
extern bool gbTransitionLogEnable;
static int s5pc110_target(struct cpufreq_policy *policy,
		       unsigned int target_freq,
		       unsigned int relation)
{
	struct cpufreq_freqs freqs;
	int ret = 0;
	unsigned long arm_clk;
	unsigned int index;

	//unsigned long irqflags;

	DBG("s5pc110_target called for freq=%d\n",target_freq);

	freqs.old = s5pc110_getspeed(0);
	DBG("old _freq = %d\n",freqs.old);

	if(policy != NULL) {
		if(policy -> governor) {
			//spin_lock_irqsave(&g_cpufreq_lock, irqflags);
			if (strnicmp(cpufreq_governor_name, policy->governor->name, CPUFREQ_NAME_LEN)) {
				strcpy(cpufreq_governor_name, policy->governor->name);
			}
			//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
		}
	}

	index = s5pc11x_target_freq_index(target_freq);
	if(index == INDX_ERROR) {
       	printk("s5pc110_target: INDX_ERROR \n");
		return -EINVAL;
        }
	DBG("Got index = %d\n",index);

	if(prevIndex == index)
	{	

		DBG("Target index = Current index\n");
       	return ret;
	}

	arm_clk = s5pc110_freq_table[S5PC11X_FREQ_TAB][index].frequency;

	freqs.new = arm_clk;
	freqs.cpu = 0;

	target_freq = arm_clk;
	cpufreq_notify_transition(&freqs, CPUFREQ_PRECHANGE);
	//spin_lock_irqsave(&g_cpufreq_lock, irqflags);

	if(prevIndex < index) { // clock down
       	dvfs_change_direction = 0;
		/* frequency scaling */
		ret = s5pc11x_clk_dsys_psys_change(index);

		ret = s5pc11x_armclk_set_rate(mpu_clk, target_freq * KHZ_T);
		//ret = clk_set_rate(mpu_clk, target_freq * KHZ_T);
		if(ret != 0) {
			printk("frequency scaling error\n");
			ret = -EINVAL;
			goto s5pc110_target_end;
		}
		
		// ARM MCS value set
		if (S5PC11X_FREQ_TAB  == 0) { // for 1G table
			if ((prevIndex < 3) && (index >= 3)) {
				ret = __raw_readl(S5P_ARM_MCS);
				DBG("MDSvalue = %08x\n", ret);
				ret = (ret & ~(0x3)) | 0x3;
				__raw_writel(ret, S5P_ARM_MCS);
			}
		} else if (S5PC11X_FREQ_TAB  == 1) { // for 1.2Ghz table
			if ((prevIndex < 4) && (index >= 4)) {
				ret = __raw_readl(S5P_ARM_MCS);
				ret = (ret & ~(0x3)) | 0x3;
				__raw_writel(ret, S5P_ARM_MCS);
			}		
		} else {
			DBG("\n\nERROR\n\n INVALID DVFS TABLE !!\n");
			return ret;
		}

/* TODO */
#if 0
		if (S5PC11X_FREQ_TAB) {	
			if (index <= 2)
				dvs_set_for_1dot2Ghz(1);
			else if (index >= 3)
				dvs_set_for_1dot2Ghz(0);
		}
#endif

#ifdef USE_DVS
#ifdef GPIO_BASED_DVS
		set_voltage_dvs(index);
#else
		/* voltage scaling */
		set_voltage(index);
#endif
#endif
		dvfs_change_direction = -1;
	}else{                                          // clock up
		dvfs_change_direction = 1;
/* TODO */
#if 0
		if (S5PC11X_FREQ_TAB) {	
			if (index <= 2)
				dvs_set_for_1dot2Ghz(1);
			else if (index >= 3)
				dvs_set_for_1dot2Ghz(0);
		}
#endif
#ifdef USE_DVS
#ifdef GPIO_BASED_DVS
		set_voltage_dvs(index);
#else
		/* voltage scaling */
		set_voltage(index);
#endif
#endif

		// ARM MCS value set
		if (S5PC11X_FREQ_TAB  == 0) { // for 1G table
			if ((prevIndex >= 3) && (index < 3)) {
				ret = __raw_readl(S5P_ARM_MCS);
				DBG("MDSvalue = %08x\n", ret);				
				ret = (ret & ~(0x3)) | 0x1;
				__raw_writel(ret, S5P_ARM_MCS);
			}
		} else if (S5PC11X_FREQ_TAB  == 1) { // for 1.2G table
			if ((prevIndex >= 4) && (index < 4)) {
				ret = __raw_readl(S5P_ARM_MCS);
				ret = (ret & ~(0x3)) | 0x1;
				__raw_writel(ret, S5P_ARM_MCS);
			}		
		} else {
			DBG("\n\nERROR\n\n INVALID DVFS TABLE !!\n");
			return ret;
		}

		/* frequency scaling */
		ret = s5pc11x_armclk_set_rate(mpu_clk, target_freq * KHZ_T);
		//ret = clk_set_rate(mpu_clk, target_freq * KHZ_T);
		if(ret != 0) {
			printk("frequency scaling error\n");
			ret = -EINVAL;
			goto s5pc110_target_end;
		}
		ret = s5pc11x_clk_dsys_psys_change(index);
		dvfs_change_direction = -1;
        }

	//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
	cpufreq_notify_transition(&freqs, CPUFREQ_POSTCHANGE);
	prevIndex = index; // save to preIndex

	mpu_clk->rate = freqs.new * KHZ_T;

#if 1 // not using it as of now
	/*change the frequency threshold level*/
	store_up_down_threshold(s5pc110_thres_table[S5PC11X_FREQ_TAB][index][0], 
				s5pc110_thres_table[S5PC11X_FREQ_TAB][index][1]);
#endif
	DBG("Perf changed[L%d] freq=%d\n",index,arm_clk);
#ifdef CONFIG_CPU_FREQ_LOG
	if(gbTransitionLogEnable == true)
	{
		DBG("Perf changed[L%d]\n",index);
		printk("[DVFS Transition]...\n");
		print_clocks();
	}
#endif
	return ret;
s5pc110_target_end:
	//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
	return ret;
}

#ifdef CONFIG_CPU_FREQ_LOG
static void inform_dvfs_clock_status(struct work_struct *work) {
//	if (prevIndex == )
	printk("[Clock Info]...\n");	
	print_clocks();
	schedule_delayed_work(&dvfs_info_print_work, 1 * HZ);
}
#endif

#ifdef CONFIG_HAS_WAKELOCK
#if 0
void s5pc11x_cpufreq_powersave(struct early_suspend *h)
{
	//unsigned long irqflags;
	//spin_lock_irqsave(&g_cpufreq_lock, irqflags);
	s5pc11x_cpufreq_level = S5PC11X_MAXFREQLEVEL + 2;
	//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
	return;
}

void s5pc11x_cpufreq_performance(struct early_suspend *h)
{
	//unsigned long irqflags;
	if(!is_userspace_gov()) {
		//spin_lock_irqsave(&g_cpufreq_lock, irqflags);
		s5pc11x_cpufreq_level = S5PC11X_MAXFREQLEVEL;
		s5pc11x_cpufreq_index = CLIP_LEVEL(s5pc11x_cpufreq_index, S5PC11X_MAXFREQLEVEL);
		//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
		s5pc110_target(NULL, s5pc110_freq_table[S5PC11X_FREQ_TAB][s5pc11x_cpufreq_index].frequency, 1);
	}
	else {
		//spin_lock_irqsave(&g_cpufreq_lock, irqflags);
		s5pc11x_cpufreq_level = S5PC11X_MAXFREQLEVEL;
		//spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);
#ifdef USE_DVS
#ifdef GPIO_BASED_DVS
		set_voltage_dvs(s5pc11x_cpufreq_index);
#else
		set_voltage(s5pc11x_cpufreq_index);
#endif
#endif
	}
	return;
}

static struct early_suspend s5pc11x_freq_suspend = {
	.suspend = s5pc11x_cpufreq_powersave,
	.resume = s5pc11x_cpufreq_performance,
	.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 1,
};
#endif
#endif //CONFIG_HAS_WAKELOCK


unsigned int get_min_cpufreq(void)
{	unsigned int frequency;
	frequency = s5pc110_freq_table[S5PC11X_FREQ_TAB][S5PC11X_MAXFREQLEVEL].frequency;
	return frequency;
}

static int __init s5pc110_cpu_init(struct cpufreq_policy *policy)
{
	u32 i;
	//unsigned long irqflags;

	mpu_clk = clk_get(NULL, MPU_CLK);
	if (IS_ERR(mpu_clk))
		return PTR_ERR(mpu_clk);

	/*set the table for maximum performance*/

	if (policy->cpu != 0)
		return -EINVAL;
	policy->cur = policy->min = policy->max = s5pc110_getspeed(0);
	//spin_lock_irqsave(&g_cpufreq_lock, irqflags);

#if USE_1DOT2GHZ
		S5PC11X_FREQ_TAB = 1;
		S5PC11X_MAXFREQLEVEL = 5;
		MAXFREQ_LEVEL_SUPPORTED = 6;
		g_dvfs_high_lock_limit = 5;
#else
		S5PC11X_FREQ_TAB = 0;
		S5PC11X_MAXFREQLEVEL = 5;
		MAXFREQ_LEVEL_SUPPORTED = 6;
		g_dvfs_high_lock_limit = 5;
#endif
	
	printk("S5PC11X_FREQ_TAB=%d , S5PC11X_MAXFREQLEVEL=%d\n",S5PC11X_FREQ_TAB,S5PC11X_MAXFREQLEVEL);

	s5pc11x_cpufreq_level = S5PC11X_MAXFREQLEVEL;
      //spin_unlock_irqrestore(&g_cpufreq_lock, irqflags);

	if (S5PC11X_FREQ_TAB) {	
		prevIndex = 2;// we are currently at 800MHZ level
	} else {
		prevIndex = 1;// we are currently at 800MHZ level
	}

#ifdef CONFIG_CPU_FREQ_LOG
	//schedule_delayed_work(&dvfs_info_print_work, 60 * HZ);
#endif
	cpufreq_frequency_table_get_attr(s5pc110_freq_table[S5PC11X_FREQ_TAB], policy->cpu);

	policy->cpuinfo.transition_latency = 80000; //40000;	//1us

#ifdef CONFIG_HAS_WAKELOCK
//	register_early_suspend(&s5pc11x_freq_suspend);	
#endif

	#if ENABLE_DVFS_LOCK_HIGH
	/*initialise the dvfs lock level table*/
	for(i = 0; i < NUMBER_OF_LOCKTOKEN; i++)
		g_dvfslockval[i] = MAXFREQ_LEVEL_SUPPORTED-1;
	#endif


	return cpufreq_frequency_table_cpuinfo(policy, s5pc110_freq_table[S5PC11X_FREQ_TAB]);
}

static struct cpufreq_driver s5pc110_driver = {
	.flags		= CPUFREQ_STICKY,
	.verify		= s5pc110_verify_speed,
	.target		= s5pc110_target,
	.get		= s5pc110_getspeed,
	.init		= s5pc110_cpu_init,
	.name		= "s5pc110",
};

static int __init s5pc110_cpufreq_init(void)
{
	return cpufreq_register_driver(&s5pc110_driver);
}

//arch_initcall(s5pc110_cpufreq_init);
module_init(s5pc110_cpufreq_init);
