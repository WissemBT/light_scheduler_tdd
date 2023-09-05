#include "LightScheduler.h"
#include "LightControl.h"
#include "TimeService.h"
#include "string.h" 

//This array can contain 256 Event
LightEvent lightEvents[256];
//This array contains the state of each light , ON/OFF/UNKNOWN
int Lights[256];
//This variable is the counter of the number of events
static int eventCounter = 0;
//This will contain a message error in case there was scheduled more than 256 event
static char SizeErr[150] = "";
//This will contain a message erro in case we tried to schedule an event for a light with ID not between 0 and 255
static char IdExceedErr[100] ="";

static int handle=-1;
//This function is for initializing the arrays to unknown state, will be called in LightScheduler init and destroy functions
void initializer(void){
	eventCounter = 0;
	for (int i=0;i<256;i++){
        Lights[i] = LIGHT_ID_UNKNOWN;
        lightEvents[i].id = LIGHT_ID_UNKNOWN;
        lightEvents[i].event = LIGHT_STATE_UNKNOWN; 
        lightEvents[i].time.dayOfWeek = NONE;
        lightEvents[i].time.minuteOfDay = -1;
	}
}
//This function is to initialize the lightscheduler
void LightScheduler_init(void){
	initializer();
	handle = TimeService_startPeriodicAlarm(60, LightScheduler_wakeUp);
}
//This function is to destory the lightscheduler
void LightScheduler_destroy(void){
	initializer();
	TimeService_stopPeriodicAlarm(handle);
	handle = -1;
	strcpy(IdExceedErr, "");
	strcpy(SizeErr, "");  
}
//This function is designed to schedule an event
void LightScheduler_schedule(int id, Time time ,int event){
	//Verify if the id is between 0 and 255
	if(id>255 || id<0){
		strcpy(IdExceedErr, "ID DOESNT EXIST");	
	}
	else{
		if(time.dayOfWeek!=NONE){
			if(time.minuteOfDay>=0 && time.minuteOfDay<1440) {
				//Verify that there will be only 256 scheduled events
				if (eventCounter>=0 && eventCounter<256){
					int exist = 0;
					for (int i=0;i<256;i++){
						//verify if event is already scheduled 
						if (lightEvents[i].id==id && lightEvents[i].event == event && lightEvents[i].time.dayOfWeek== time.dayOfWeek && lightEvents[i].time.minuteOfDay == time.minuteOfDay){
							exist = 1;
							break;
						}
					}
					//if event is not already scheduled , scheduled it
					if(!exist){
						lightEvents[eventCounter].id=id;
						lightEvents[eventCounter].event = event;
						lightEvents[eventCounter].time.dayOfWeek= time.dayOfWeek;
						lightEvents[eventCounter].time.minuteOfDay = time.minuteOfDay;
						eventCounter++;
						}
					}
				else{     
        	    		strcpy(SizeErr, "Too many events!");
				}
			}
		}
	}
}
//This function is designed to unschedule an already scheduled event,if the event doesn't exist, nothing will happen
void LightScheduler_unschedule(int id, Time time, int event){
	for (int i=0;i<256;i++){
		//search for the specified event, if exist swap the last scheduled event with it and after that unschedule it by putting all its attributes to unknown state
		if(lightEvents[i].id==id && lightEvents[i].event==event && lightEvents[i].time.dayOfWeek == time.dayOfWeek && lightEvents[i].time.minuteOfDay==time.minuteOfDay){
			LightEvent tmp;
			tmp.id = LIGHT_ID_UNKNOWN;
			tmp.event = LIGHT_STATE_UNKNOWN;
			tmp.time.dayOfWeek = NONE;
			tmp.time.minuteOfDay = -1;
			lightEvents[i] = lightEvents[eventCounter-1];
			lightEvents[eventCounter-1] = tmp;
			eventCounter--;
			break;
			}
	}
}
//This event will be called every 60 seconds , it will see if there's a scheduled event that need to be executed , if yes it will execute it for one minute
void LightScheduler_wakeUp(void){
    //This variable is the current time, it will be used to trigger the lights when needed
	Time currentTime;
    TimeService_getTime(&currentTime);
	//iterate over the lightEvents table to verify if there's a scheduled event
    for(int i=0; i<256; i++){
        //Verifying that there's a scheduled event
		if(lightEvents[i].id!=LIGHT_ID_UNKNOWN){ 
			//if the event is scheduled everyday or the current time is equal to the scheduled time , execute it
			if ((lightEvents[i].time.dayOfWeek == EVERYDAY || lightEvents[i].time.dayOfWeek == currentTime.dayOfWeek) 
			&& lightEvents[i].time.minuteOfDay == currentTime.minuteOfDay){
            	    	if(lightEvents[i].event == LIGHT_ON){
                	    	LightControl_on(lightEvents[i].id);
                		} else if(lightEvents[i].event == LIGHT_OFF){
                    		LightControl_off(lightEvents[i].id);
               	 		}
			}
			//if the event is scheduled only on weekend or sunday/saturday, execute it
			else if (lightEvents[i].time.dayOfWeek == WEEKEND && (currentTime.dayOfWeek == SUNDAY || currentTime.dayOfWeek == SATURDAY) && lightEvents[i].time.minuteOfDay == currentTime.minuteOfDay){
				if(lightEvents[i].event == LIGHT_ON){
                	        LightControl_on(lightEvents[i].id);
                    	    } else if(lightEvents[i].event == LIGHT_OFF){
                        	LightControl_off(lightEvents[i].id);
                        	}
			}
			//if the event is scheduled only on weekdays or on day not sunday/saturday, execute it
			else if (lightEvents[i].time.dayOfWeek == WEEKDAY && 
				(currentTime.dayOfWeek == MONDAY || currentTime.dayOfWeek == TUESDAY || currentTime.dayOfWeek== WEDNESDAY 
				|| currentTime.dayOfWeek == THURDSDAY || currentTime.dayOfWeek == FRIDAY ) && lightEvents[i].time.minuteOfDay == currentTime.minuteOfDay) {
                	        if(lightEvents[i].event == LIGHT_ON){
                    	    LightControl_on(lightEvents[i].id);
                        	} else if(lightEvents[i].event == LIGHT_OFF){
                       	 	LightControl_off(lightEvents[i].id);
                       	 	}
                	}
		}
	}
	//Here when wakeUp will be called , it needs to verify if there was an event that was executed a minute ago, if yes , turn it back to unknown state
	for (int i=0;i<256;i++){
		if ((lightEvents[i].time.dayOfWeek == EVERYDAY || lightEvents[i].time.dayOfWeek == WEEKEND 
		|| lightEvents[i].time.dayOfWeek == WEEKDAY || lightEvents[i].time.dayOfWeek == currentTime.dayOfWeek) && lightEvents[i].time.minuteOfDay == currentTime.minuteOfDay -1) {
			Lights[lightEvents[i].id] = LIGHT_STATE_UNKNOWN;
		}
	} 
}
//event counter getter
int LightScheduler_getEventCounter(void) {
    return eventCounter;
}
//size error getter
const char* LightScheduler_getSizeError(void) {
    return SizeErr;
}
//id error getter
const char* LightScheduler_getIdExceededError(void){
    return IdExceedErr;
}
