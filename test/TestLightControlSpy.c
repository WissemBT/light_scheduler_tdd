#include "LightControlSpy.h"
#include "unity.h"

void testLightControlSpyReturnsLastStateChange(void)
{
    LightControl_init();
    LightControl_on(42);
    
    TEST_ASSERT_EQUAL( 42, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL( LIGHT_ON, LightControlSpy_getLastState() );

    LightControl_destroy();
}

//This test is to verify if the init returns Unknown after creation
void testLightControlSpyReturnsUnknownAfterCreation(void)
{
    LightControl_init();
    TEST_ASSERT_EQUAL(LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId()); 
    TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState()); 
    LightControl_destroy();
}
