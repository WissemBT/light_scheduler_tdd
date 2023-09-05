#include "LightScheduler.h"
#include "unity.h"
#include "MockTimeService.h"
#include "LightControlSpy.h"

extern LightEvent lightEvents[256];
extern int Lights[256];
extern int eventCounter;

void setUp(void)
{
        //The following functions are called because I kept getting an error whenever I test
	TimeService_init_Ignore();
    	TimeService_startPeriodicAlarm_IgnoreAndReturn(0);
    	TimeService_stopPeriodicAlarm_Ignore();
    	TimeService_destroy_Ignore();	

	TimeService_init();
	LightControl_init();
	LightScheduler_init();

}

void tearDown(void)
{
	TimeService_destroy();
	LightControl_destroy();
	LightScheduler_destroy();
}
//This test is to test if event counter is 0 after initialization
void testEventCounterIsZero(void){
	TEST_ASSERT_EQUAL(0,LightScheduler_getEventCounter());
}
//This test is to test that there's no event and states of lights after initialization
void testThatNoEventsAndStatesAfterCreation(void){
	for (int i = 0; i < 256; i++){
	TEST_ASSERT_EQUAL(LIGHT_ID_UNKNOWN, Lights[i]);
        TEST_ASSERT_EQUAL(LIGHT_ID_UNKNOWN, lightEvents[i].id);
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN, lightEvents[i].event);
        TEST_ASSERT_EQUAL(NONE, lightEvents[i].time.dayOfWeek);
        TEST_ASSERT_EQUAL(-1, lightEvents[i].time.minuteOfDay);
    	}
}
//This test is to verify that an event can be scheduled successfully
void testEventIsScheduledOnce(void){
	int notYet = 0;
	Time t;
	t.dayOfWeek = MONDAY;
	t.minuteOfDay = 1200;
	LightScheduler_schedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
		if(lightEvents[i].id==0){
		TEST_ASSERT_EQUAL(0, lightEvents[i].id);
        	TEST_ASSERT_EQUAL(LIGHT_ON, lightEvents[i].event);
        	TEST_ASSERT_EQUAL(MONDAY, lightEvents[i].time.dayOfWeek);
        	TEST_ASSERT_EQUAL(1200, lightEvents[i].time.minuteOfDay);
		notYet = 1;
		break;
		}
	}
	TEST_ASSERT_EQUAL(1,notYet);
}
//This test is to verify that we can't schedule an event if ID>255
void testEventScheduledForBiggerID(void){
	Time t;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(600,t,LIGHT_ON);
	TEST_ASSERT_EQUAL_STRING("ID DOESNT EXIST",LightScheduler_getIdExceededError());
}
//This test is to verify that we can't schedule an event if ID<0 
void testEventScheduledForNegativeID(void){
        Time t;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(-5,t,LIGHT_ON);
        TEST_ASSERT_EQUAL_STRING("ID DOESNT EXIST",LightScheduler_getIdExceededError());
}

//This test is to verify that a scheduled event can be unscheduled successfully
void testEventIsUnscheduledOnce(void){
	Time t;
	int exist = 0;
	t.dayOfWeek = MONDAY;
	t.minuteOfDay = 1200;
	LightScheduler_schedule(0,t,LIGHT_ON);
	LightScheduler_unschedule(0,t,LIGHT_ON);
	for(int i=0;i<256;i++){
		if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay==1200 )
			exist = 1;
}
	TEST_ASSERT_EQUAL(0,exist);
}
//This test is to see what happens when we unschedule an unscheduled event
void testToUnscheduleAnUnscheduledEvent(void){
        Time t;
        int exist = 0;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_unschedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
                if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay==1200 )
                        exist = 1;
}
        TEST_ASSERT_EQUAL(0,exist);
}
//This test is to schedule and unschedule for different time for the same id
void testEventUnscheduleScheduledDifferentTimeOnce(void){
        Time t;
        int exist = 0;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_ON);
	t.minuteOfDay = 1300;
        LightScheduler_unschedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
                if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay==1200 )
                        exist = 1;
}
        TEST_ASSERT_EQUAL(1,exist);
}

//This test is to verify that we can schedule 256 successfully
void testCanSchedule256Events(void){
	Time t;
	t.dayOfWeek= MONDAY;
	t.minuteOfDay=10;
	for(int i=0;i<256;i++){
	LightScheduler_schedule(0,t,LIGHT_ON);
	t.minuteOfDay++;
	}
	TEST_ASSERT_EQUAL(256,LightScheduler_getEventCounter());
}
//This test is to verify that we can't go beyond 256 events
void testCantGoBeyond256Events(void){
        Time t;
        t.dayOfWeek= MONDAY;
        t.minuteOfDay=10;
        for(int i=0;i<260;i++){
        LightScheduler_schedule(0,t,LIGHT_ON);
        t.minuteOfDay++;
        }
        TEST_ASSERT_EQUAL_STRING("Too many events!", LightScheduler_getSizeError());
}
//This test is to verify that we can schedule multiple different events for the same light
void testMultipleEventsForOneLight(void)
{
    Time t;
    t.dayOfWeek = MONDAY;
    t.minuteOfDay = 1200;
    LightScheduler_schedule(1, t, LIGHT_ON);
    t.minuteOfDay++;
    LightScheduler_schedule(1, t, LIGHT_ON);
    t.minuteOfDay++;
    LightScheduler_schedule(1, t, LIGHT_ON);
    int cpt = 0;
    for (int i = 0; i < 256; i++) {
        if (lightEvents[i].id == 1) cpt++;
    }
    TEST_ASSERT_EQUAL(3, cpt);
}
//This test is to verify that we can't schedule the same event twice and event is only scheduled once
void testEventScheduledOnlyOncePerLight(void)
{
    	Time t;
    	t.dayOfWeek = MONDAY;
    	t.minuteOfDay = 1200;
    	LightScheduler_schedule(1, t, LIGHT_ON);
    	LightScheduler_schedule(1, t, LIGHT_ON);
	int cpt = 0;
	for (int i=0;i<256;i++) {
		if (lightEvents[i].id == 1 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay ==1200)
			cpt++;
}
	TEST_ASSERT_EQUAL(1,cpt);
}
//This test is to verify that we can schedule a turn on event on a specific day and time and it will be executed successfully
void testTurnOnLightOnSpecificDayAndTime(void) {
        TimeService_getTime_ExpectAnyArgs();
	Time t;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_ON);
	TimeService_getTime_ReturnThruPtr_time(&t);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
}
//This test is to verify that we can schedule a turn off event on a specific day and time and it will be executed successfully
void testTurnOffLightOnSpecificDayAndTime(void){
	TimeService_getTime_ExpectAnyArgs();
        Time t;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_OFF);
        TimeService_getTime_ReturnThruPtr_time(&t);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
}
//This event is to verify that we can turn on/off multiple time the same light on specific time
//in this test i turned on the light twice and turn off once on different time and each time it was executed successfully
void testTurnOnMultipleTimesOnOffSpecificDayAndTimeOneLight(void) {
        TimeService_getTime_ExpectAnyArgs();
        Time t,t1,t2;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
	t1.dayOfWeek = TUESDAY;
        t1.minuteOfDay = 1400;
	t2.dayOfWeek = MONDAY;
        t2.minuteOfDay = 1420;
        LightScheduler_schedule(0,t,LIGHT_ON);
	LightScheduler_schedule(0,t1,LIGHT_OFF);
	LightScheduler_schedule(0,t2,LIGHT_ON);
        TimeService_getTime_ReturnThruPtr_time(&t);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
       	TimeService_getTime_ExpectAnyArgs();
	TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
	TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
	TimeService_getTime_ExpectAnyArgs();
	TimeService_getTime_ReturnThruPtr_time(&t2);
	LightScheduler_wakeUp();
	TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
}
//This test is to verify that the light is triggered only for one minute after that it will be back to unknown state
void testEventTriggeredOnlyForOneMinute(void){
	TimeService_getTime_ExpectAnyArgs();
        Time t;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_ON);
        TimeService_getTime_ReturnThruPtr_time(&t);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
	t.minuteOfDay = 1201;
	TimeService_getTime_ExpectAnyArgs();
	TimeService_getTime_ReturnThruPtr_time(&t);
	LightScheduler_wakeUp();
	TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
}
//Test to verify that we can schedule an event every day
void testEventIsScheduledEveryDay(void){
	Time t;
	t.dayOfWeek = EVERYDAY;
	t.minuteOfDay = 1200;
	LightScheduler_schedule(0,t,LIGHT_ON);
	Time t1;
	t1.minuteOfDay = 1200;
	t1.dayOfWeek = MONDAY;
	TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
	t1.dayOfWeek = WEDNESDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
	t1.dayOfWeek = SUNDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
}
//Two tests to verify that we can schedule an event light on/off only on weekend
void testEventONIsScheduledOnlyOnWeekEnd(void){
	Time t;
        t.dayOfWeek = WEEKEND;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_ON);
        Time t1;
        t1.minuteOfDay = 1200;
        t1.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = SATURDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
        t1.dayOfWeek = SUNDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
}
void testEventOFFIsScheduledOnlyOnWeekEnd(void){
        Time t;
        t.dayOfWeek = WEEKEND;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_OFF);
        Time t1;
        t1.minuteOfDay = 1200;
        t1.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = SATURDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
        t1.dayOfWeek = SUNDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
}
//Two Tests to verify that we can schedule an event light on/off only on weekdays
void testEventONIsScheduledOnlyOnWeekDays(void){
        Time t;
        t.dayOfWeek = WEEKDAY;
        t.minuteOfDay = 1000;
        LightScheduler_schedule(0,t,LIGHT_ON);
        Time t1;
        t1.minuteOfDay = 1000;
        t1.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
	Time t2;
	t2.minuteOfDay = 1001;
        t2.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t2);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = SATURDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = FRIDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_ON,Lights[0]);
}
void testEventOFFIsScheduledOnlyOnWeekDays(void){
        Time t;
        t.dayOfWeek = WEEKDAY;
        t.minuteOfDay = 1000;
        LightScheduler_schedule(0,t,LIGHT_OFF);
        Time t1;
        t1.minuteOfDay = 1000;
        t1.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
        Time t2;
        t2.minuteOfDay = 1001;
        t2.dayOfWeek = MONDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t2);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = SATURDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,Lights[0]);
        t1.dayOfWeek = FRIDAY;
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&t1);
        LightScheduler_wakeUp();
        TEST_ASSERT_EQUAL(LIGHT_OFF,Lights[0]);
}

//This Test is to verify that we can't schedule an event for an inexistant day
void testEventCantBeScheduledForNoneDay(void){
	Time t;
        int exist = 0;
        t.dayOfWeek = NONE;
        t.minuteOfDay = 1200;
        LightScheduler_schedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
                if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == NONE && lightEvents[i].time.minuteOfDay==1200 )
                        exist = 1;
}
        TEST_ASSERT_EQUAL(0,exist);	
}
//This Test is to verify that we can't schedule an event for a negative minute of day
void testEventCantBeScheduledForNegativeMinute(void){
        Time t;
        int exist = 0;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = -5;
        LightScheduler_schedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
                if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay==-5 )
                        exist = 1;
}
        TEST_ASSERT_EQUAL(0,exist);     
}
//This Test is to verify that we can't schedule an event for a negative minute of day
void testEventCantBeScheduledForSurpassingDayMinute(void){
        Time t;
        int exist = 0;
        t.dayOfWeek = MONDAY;
        t.minuteOfDay = 1800;
        LightScheduler_schedule(0,t,LIGHT_ON);
        for(int i=0;i<256;i++){
                if(lightEvents[i].id == 0 && lightEvents[i].event== LIGHT_ON && lightEvents[i].time.dayOfWeek == MONDAY && lightEvents[i].time.minuteOfDay==1800)
                        exist = 1;
}
        TEST_ASSERT_EQUAL(0,exist);     
}

//void testThatFailsBecauseItDoesNotTestAnything(void)
//{
 //   TEST_ASSERT_MESSAGE( 0, "Go write some real tests!" );
//}

