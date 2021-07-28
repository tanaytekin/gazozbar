#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <time.h>
#include <string.h>


typedef struct Module
{
        char str[128];
        void (*update)(int dummy);
}Module;

typedef struct ModuleSignal
{
        Module *module;
        int signal;
}ModuleSignal;


static void memory_update(int dummy);


static Module memory_m = {{'\0'}, memory_update};


static void memory_update(int dummy)
{
        FILE *fp;
        int tmp;
        float total;
        float used;

        fp = popen("free | grep Mem | awk '{print $2}'", "r");
        fscanf(fp, "%d", &tmp);
        pclose(fp);

        total = tmp * 1.0f / 1024 / 1024;

        fp = popen("free | grep Mem | awk '{print $3}'", "r");
        fscanf(fp, "%d", &tmp);
        pclose(fp);

        used = tmp * 1.0f / 1024 / 1024;

        sprintf(memory_m.str, "[ %.2fG/%.2fG]", used, total);
}


static void time_update(int dummy);

static Module time_m = {{'\0'}, time_update};

static void time_update(int dummy)
{
        time_t t;
        struct tm *tm;
        
        t = time(NULL);
        tm = localtime(&t);

        strftime(time_m.str, 128, "[ %d %B %Y - %H:%M]", tm);
}




static void pulse_update(int dummy);

static Module pulse_m ={{'\0'}, pulse_update};

static void pulse_update(int dummy)
{
        FILE *fp;
        int volume;
        char buf[10];
        char *icons[] = 	{"婢","奄","奔","墳"};
        int icon;

        icon = 0;
        volume = 0;


        fp = popen("/bin/pamixer --get-volume", "r");
        fscanf(fp, "%d", &volume);
        pclose(fp);
        fp = NULL;
        fp = popen("/bin/pamixer --get-volume-human", "r");
        fscanf(fp, "%s", buf);
        pclose(fp);



        if(strcmp(buf,"muted") == 0)
        {
                icon = 0;
        }
        else
        {
                if(volume <= 33)
                {
                        icon = 1;
                }
                else if(volume <= 66)
                {
                        icon = 2;
                }
                else
                {
                        icon = 3;
                }
        }

        sprintf(pulse_m.str,"[%s %d%%]", icons[icon], volume);

}







static void battery_update(int dummy);

static Module battery_m ={{'\0'}, battery_update};


extern char *battery_status;
extern char *battery_capacity;

static void battery_update(int dummy)
{
	char *icons[] = { "","","","","","","","","","","","","","","","",""};

	FILE *fp;
	char status[32];
	int capacity;
	int icon;

	capacity = 0;
	icon = 0;

	fp = fopen(battery_status, "r");
	fscanf(fp,"%s", status);
	fclose(fp);
	fp = NULL;
	
	fp = fopen(battery_capacity, "r");
	fscanf(fp,"%d", &capacity);
	fclose(fp);

	if(strcmp(status,"Discharging") == 0)
	{
		if(capacity <= 10) { icon = 7;}
		else if(capacity <= 20) { icon = 8;}
		else if(capacity <= 30) { icon = 9;}
		else if(capacity <= 40) { icon = 10;}
		else if(capacity <= 50) { icon = 11;}
		else if(capacity <= 60) { icon = 12;}
		else if(capacity <= 70) { icon = 13;}
		else if(capacity <= 80) { icon = 14;}
		else if(capacity <= 90) { icon = 15;}
		else                    { icon = 16;}
	}
	else
	{
		if(capacity <= 20)      { icon = 0;}
		else if(capacity <= 30) { icon = 1;}
		else if(capacity <= 40) { icon = 2;}
		else if(capacity <= 60) { icon = 3;}
		else if(capacity <= 80) { icon = 4;}
		else if(capacity <= 90) { icon = 5;}
		else                    { icon = 6;}
	}

	sprintf(battery_m.str, "[%s %d%%]", icons[icon], capacity);


}


static void backlight_update(int dummy);

static Module backlight_m ={{'\0'}, backlight_update};

static void backlight_update(int dummy)
{
        FILE *fp;
        int brightness;

	brightness = 0;

        fp = popen("xbacklight -get", "r");
        fscanf(fp, "%d", &brightness);
        pclose(fp);
        
	sprintf(backlight_m.str, "[%s %d%%]", "", brightness);

}


#include "config.h"





int main()
{
        char bar_str[128];
        int i;
        int module_count;
        int module_with_signal_count;

        Display *display;


        display = XOpenDisplay(NULL);
        if(!display)
        {
                fprintf(stderr, "Cannot open display.\n");
                return 1;
        }
        
        module_count = sizeof(modules)/sizeof(Module*);
        module_with_signal_count = sizeof(modules_with_signal)/sizeof(ModuleSignal);
       

        
        for(i=0; i<module_with_signal_count; i++)
        {
                if(modules_with_signal[i].signal > 0)
                {
                        signal(SIGRTMIN + modules_with_signal[i].signal ,
                                        modules_with_signal[i].module->update);
                }
        }

        while(1)
        {


                
                memset(bar_str, 0, 128);
                

                for(i=0; i<module_count; i++)
                {
                        modules[i]->update(0);
                        strcat(bar_str, modules[i]->str);
                }

                XStoreName(display, DefaultRootWindow(display), bar_str);
                XSync(display, False);
                sleep(update_interval);
        }



        return 0;
}
