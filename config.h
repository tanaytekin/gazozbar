static int update_interval = 10;


char *battery_status = "/sys/class/power_supply/BAT0/status";
char *battery_capacity = "/sys/class/power_supply/BAT0/capacity";



static Module *modules[] = {  &memory_m, &pulse_m, &backlight_m, &battery_m, &time_m };
static ModuleSignal modules_with_signal[] = {{&pulse_m, 2}, {&backlight_m, 3}}; 
