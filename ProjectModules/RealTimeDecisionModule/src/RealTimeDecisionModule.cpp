#include "RealTimeDecisionModule.h"

int main()
{
/* initiate url, user, password and database variables */
    sqlInit();
    MessageServerFunc::socketOpen();
    while(1)
    { 
      MajorFlowFunc::conveyorJobCreation();
    }
    return 0;
}