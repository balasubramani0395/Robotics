#include "SequenceFlows.h"

namespace threadParam
{
    static std::mutex dbMutex, MSMutex, mapMutex, FRMutex;
    std::mutex SCMutex;
    std::map<int, std::vector<string>> robotStatusMap;
    std::vector<string> scDeptList;
} // namespace threadParam

namespace MajorFlowFunc
{
    void conveyorDirectionChange()
    {
        while (1)
        {

            system_clock::time_point currentTime = std::chrono::system_clock::now();
            time_t tt = system_clock::to_time_t(currentTime);
            tm local_tm = *localtime(&tt);
            int currTime;
            currTime = (local_tm.tm_hour * 100) + local_tm.tm_min;
            if (ScheduleReadInit == 0)
            {
                jsonSchedule = "";
                if (ScheduleReadInit != 1)
                {
                    jsonSchedule = MessagesFunc::scheduleWindowClosed("");
                    cout << "jsonSchedule" << jsonSchedule << endl;
                }
                if (jsonSchedule != "")
                {
                    ScheduleReadInit = 1;
                }
            }

            if (ScheduleRead == 0 && currTime == 1725)
            {
                jsonSchedule = "";
                if (ScheduleRead != 1)
                {
                    jsonSchedule = MessagesFunc::scheduleWindowClosed("");
                    cout << "jsonSchedule" << jsonSchedule << endl;
                }
                if (jsonSchedule != "")
                {
                    ScheduleRead = 1;
                }
            }
            else if (currTime != 600 && ScheduleRead == 1)
            {
                ScheduleRead = 0;
            }

            std::map<string, std::vector<string>> WHCScheduleMap;
            Json::Reader reader;
            Json::Value root;
            std::ifstream cfgfile2("/home/stk-ait02/Documents/WHC/ProjectModules/Integration_testing2/schedule.json");
            

            reader.parse(jsonSchedule, root, false);
            auto entriesArray = root["timeslots"];
            std::vector<string> WHCScheduleVector;
            WHCScheduleVector.clear();
            for (int i = 0; i < entriesArray.size(); i++)
            {
                auto firstelem = entriesArray[i];
                if (firstelem["start_time"].asString().size() != 0 && firstelem["end_time"].asString().size() != 0)
                {
                    WHCScheduleVector.push_back(firstelem["start_time"].asString());
                    WHCScheduleVector.push_back(firstelem["end_time"].asString());
                    WHCScheduleVector.push_back(firstelem["trolleys"].asString());
                    WHCScheduleMap[firstelem["department"].asString()] = WHCScheduleVector;
                }
            }

            std::string startTime, endTime;
            int startTimeHr, endTimeHr;
            threadParam::SCMutex.lock();
            threadParam::scDeptList.clear();
            if(WHCScheduleMap.size() == 0)
            {
                cout << "Schedule is not updated from WHC API" << endl;
            }
            for (std::map<string, std::vector<string>>::iterator it = WHCScheduleMap.begin(); it != WHCScheduleMap.end(); ++it)
            {
                std::vector<string> tempVector = it->second;
                string tempVector2 = it->first;
                int i = 0;
                for (auto iterator = tempVector.end() - 3; iterator != tempVector.end(); ++iterator)
                {
                    if (i == 0)
                        startTime = *iterator;
                    else if (i == 1)
                        endTime = *iterator;
                    i++;
                }
                startTimeHr = (stoi(startTime.substr(0, 2)) * 100) + stoi(startTime.substr(3, startTime.length()));
                endTimeHr = (stoi(endTime.substr(0, 2)) * 100) + stoi(endTime.substr(3, endTime.length()));

                if ((startTimeHr < currTime) && (endTimeHr > currTime))
                {
                    threadParam::scDeptList.push_back(it->first);
                }

            }
            threadParam::SCMutex.unlock();
            usleep(10000000);
        }
    }
    string scheduleWindowClosed(string deptID)
    {
        string result = "";
        cout << "Department check : " << deptID << endl;
        const std::lock_guard<std::mutex> lock(threadParam::SCMutex);
        for (auto iterator = threadParam::scDeptList.begin(); iterator < threadParam::scDeptList.end(); ++iterator)
        {
            cout << "Department check : " << deptID << " Department in Scedule " << *iterator << endl;
            if (deptID == *iterator)
            {
                result = "closed";
                return result;
            }
            else
            {
                result = "open";
            }
        }
        cout << "Department check : " << deptID << endl;
        return result;
    }
    void conveyorJobCreation()
    {

        std::queue<std::pair<string, string>> SQLdbUpdateQueue;
        std::queue<std::pair<string, string>> returnDBUpdateQueue;
        std::tuple<int, string, int> jobData, prevJobData;
        threadParam::MSMutex.lock();
        MessageServerFunc::receiveConveyor(SQLdbUpdateQueue);
        MessageServerFunc::receivePushButton(returnDBUpdateQueue);
        int jID, jType, jIDp, jTypep;
        std::string cID, cIDp;
        threadParam::MSMutex.unlock();
        threadParam::dbMutex.lock();
        if (SQLdbUpdateQueue.empty() != true)
        {
            updateScheduleJobDB(SQLdbUpdateQueue);
        }
        if (returnDBUpdateQueue.empty() != true)
        {
            updateReturnJobDB(returnDBUpdateQueue);
        }
        jobData = checkNewJobAvaible();
        threadParam::dbMutex.unlock();
        std::tie(jID, cID, jType) = jobData;
        std::tie(jIDp, cIDp, jTypep) = prevJobData;

        if ((jID != 0) && (jID != jIDp) && initVal == 1)
        {
            if (jType == 1)
            {
                std::vector<std::thread *> schedule_threads;
                std::thread *t;

                cout << "Schedule Job created with Job ID " << jID << endl
                     << endl;

                t = new thread(&MajorFlowFunc::scheduleDispatchFlow, jID, cID);
                schedule_threads.push_back(move(t));
            }
            else if (jType == 2)
            {
                std::vector<std::thread *> return_threads;
                std::thread *t;

                cout << "Return Job created with Job ID " << jID << endl
                     << endl;

                t = new thread(&MajorFlowFunc::returnTrolleyFlow, jID, cID);

                return_threads.push_back(move(t));
            }
        }
        prevJobData = jobData;

        usleep(1000000);
        if (initVal == 0)
        {
            std::vector<std::thread *> mapThreads;
            std::thread *te;
            te = new thread(&MajorFlowFunc::conveyorDirectionChange);
            mapThreads.push_back(move(te));
            initVal = 1;
        }
    }

    bool checkPointAvailable(string goalPoint)
    {
        threadParam::mapMutex.lock();
        for (auto elem : threadParam::robotStatusMap)
        {
            string robotState = "";
            int index = 0;
            for (auto i = elem.second.begin(); i != elem.second.end(); ++i)
            {
                if (index == 1)
                {
                    robotState = *i;
                    std::size_t found = robotState.find(goalPoint);
                    if (found != std::string::npos)
                    {
                        return false;
                    }
                }
            }
        }
        return true;

        threadParam::mapMutex.unlock();
    }

    void printMapData()
    {

        int wl = 0;
        while (1)
        {
            int index = 0;
            string robotState = "";
            int x = 0;
            int y = 0;
            threadParam::mapMutex.lock();
            for (auto elem : threadParam::robotStatusMap)
            {

                for (auto i = elem.second.begin(); i != elem.second.end(); ++i)
                {
                    index++;
                    if (index == 1)
                    {
                        robotState = *i;
                    }
                    else if (index == 2)
                    {
                        x = std::stoi(*i);
                    }
                    else if (index == 3)
                    {
                        y = std::stoi(*i);
                    }
                }
                index = 0;
            }
            threadParam::mapMutex.unlock();
            if (x > 9 && x < 61 && y > 4 && y < 41)
            {
                if (wl != 1)
                {
                    cout << "------------------------------------------Turn On Warning Light " << endl;
                    threadParam::MSMutex.lock();
                    MessagesFunc::turnONWarningLight("WL1", 1);
                    threadParam::MSMutex.unlock();
                    wl = 1;
                }
            }
            else
            {
                if (wl != 0)
                {
                    cout << "------------------------------------------Turn Off Warning Light " << endl;
                    threadParam::MSMutex.lock();
                    MessagesFunc::turnONWarningLight("WL1", 0);
                    threadParam::MSMutex.unlock();
                    wl = 0;
                }
            }
            usleep(10000000);
        }
    }
    void robotStatusUpdate(string taskID, int jobID)
    {
        xPos += 10;
        yPos += 5;
        std::vector<string> robotStatusVector;
        robotStatusVector.clear();
        robotStatusVector.push_back(taskID);
        robotStatusVector.push_back(std::to_string(xPos));
        robotStatusVector.push_back(std::to_string(yPos));
        threadParam::mapMutex.lock();
        threadParam::robotStatusMap[jobID] = robotStatusVector;
        threadParam::mapMutex.unlock();
        if (xPos > 100)
            xPos = 0;
        if (yPos > 50)
            yPos = 0;
    }

    void rfidCheck(string conveyorID, string departmentID, string jobType)
    {
        string trolleyID, weight;
        if (departmentID == "Kitchen")
        {
            trolleyID = "Food" + conveyorID.substr(conveyorID.length() - 1, conveyorID.length());
        }
        else if (departmentID == "Return")
        {
            trolleyID = departmentID;
        }
        else
        {
            trolleyID = departmentID + conveyorID.substr(conveyorID.length() - 1, conveyorID.length());
        }

        std::cout << std::setw(10) << "RFID "
                  << "| WHC781964Z7500R"
                  << " " << std::endl;
        std::cout << std::setw(10) << "DEPT "
                  << "| " << departmentID << " " << std::endl;
        std::cout << std::setw(10) << "Trolley "
                  << "| " << trolleyID << " " << std::endl;
        if (jobType == "Scheduled")
            weight = "106 Kg";
        else
            weight = "50 Kg";
        std::cout << std::setw(10) << "Weight "
                  << "| " << weight << " " << std::endl;
    }

    static void scheduleDispatchFlow(const int jobID, const string conveyorID)
    {

        string departmentID;
        if (conveyorID.length() > 0)
        {
            departmentID = conveyorID.substr(0, conveyorID.length() - 1);
        }
        if (jobID != 0)
        {
            std::cout << "|==========================================================|" << std::endl;
            cout << "Schedule Window Closed Check " << jobID << endl;
            std::cout << "|==========================================================|" << std::endl;
            threadParam::MSMutex.lock();
            string SW = scheduleWindowClosed(departmentID);
            threadParam::MSMutex.unlock();
            if (SW == "closed")
            {
                std::cout << "|==========================================================|" << std::endl;
                cout << "AGV Available Check " << jobID << endl;
                std::cout << "|==========================================================|" << std::endl;
                std::pair<int, int> pool;
                threadParam::FRMutex.lock();
                pool = MessagesFunc::checkAGVPoolID(departmentID);
                std::cout << departmentID << " " << pool.first << " " << pool.second << std::endl;
                threadParam::FRMutex.unlock();
                threadParam::MSMutex.lock();
                string tugID = MessagesFunc::agvAvailable(departmentID, pool.first);
                if ((tugID == "Unavailable"))
                {
                    tugID = MessagesFunc::agvAvailable(departmentID, pool.second);
                }
                threadParam::MSMutex.unlock();
                if ((tugID != "") && (tugID != "Unavailable") && (tugID != "Failure"))
                {
                    string goalDestination, goalDestinationBlock, taskID, robotStatus, WHCJobID;
                    vector<string> destInfo;
                    cout << "AGV Available" << endl
                         << endl;
                    threadParam::dbMutex.lock();
                    updateJobActive(jobID, tugID);
                    threadParam::dbMutex.unlock();
                    taskID = conveyorID;
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Going to conveyor " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        rfidCheck(conveyorID, departmentID, "Scheduled");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached conveyor " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    usleep(5000);
                    string destCheck;
                    threadParam::MSMutex.lock();
                    destInfo = MessagesFunc::getDestination(departmentID,conveyorID);
                    WHCJobID = destInfo.back();
                    destInfo.pop_back();
                    goalDestination = destInfo.back();
                    destInfo.pop_back();
                    goalDestinationBlock = destInfo.back();
                    destCheck = MessagesFunc::mapDestination(goalDestination);
                    threadParam::MSMutex.unlock();
                    bool pointAvailable = false;
                    if ((goalDestination != "") && (destCheck != "") && (destCheck != "Unavailable") && (destCheck != "Failure"))
                    {
                        string jobDelete="";
                        int loopCount = 0;
                        while (jobDelete != "Success" || loopCount == 3)
                        {
                            threadParam::MSMutex.lock();
                            jobDelete = MessagesFunc::jobDeleteRequest(stoi(WHCJobID), departmentID);
                            threadParam::MSMutex.unlock();
                            loopCount++;
                        }
                        string bayLobbyAvailable;
                        threadParam::MSMutex.lock();
                        bayLobbyAvailable = MessagesFunc::BayandLobbyAvailable(goalDestination, departmentID);
                        threadParam::MSMutex.unlock();
                        threadParam::dbMutex.lock();
                        updateJobDestination(jobID, goalDestination, bayLobbyAvailable);
                        threadParam::dbMutex.unlock();
                        if ((bayLobbyAvailable != "") && (bayLobbyAvailable != "Occupied") && (bayLobbyAvailable != "Failure"))
                        {
                            taskID = goalDestinationBlock + "liftpoint";
                            threadParam::MSMutex.lock();
                            MessagesFunc::dispatchAGV(taskID, tugID);
                            threadParam::MSMutex.unlock();
                            std::cout << "|==========================================================|" << std::endl;
                            cout << "Going to lift " << jobID << endl;
                            std::cout << "|==========================================================|" << std::endl;
                            usleep(3000000);
                            robotStatus = "";
                            while (robotStatus != "At_Destination")
                            {
                                threadParam::MSMutex.lock();
                                robotStatus = MessagesFunc::checkRobotStatus(tugID);
                                threadParam::MSMutex.unlock();
                                robotStatusUpdate(taskID, jobID);
                                cout << taskID << endl;
                                rfidCheck(conveyorID, departmentID, "Scheduled");
                                usleep(1 * 3000000);
                            }

                            std::cout << "|==========================================================|" << std::endl;
                            cout << "Reached lift " << jobID << endl;
                            std::cout << "|==========================================================|" << std::endl;
                            threadParam::MSMutex.lock();
                            bayLobbyAvailable = MessagesFunc::BayandLobbyAvailable(goalDestination, departmentID);
                            threadParam::MSMutex.unlock();
                            if ((bayLobbyAvailable != "") && (bayLobbyAvailable != "Occupied") && (bayLobbyAvailable != "Failure"))
                            {
                                std::cout << "|==========================================================|" << std::endl;
                                cout << "Check Bay Holding Point Available " << jobID << endl;
                                std::cout << "|==========================================================|" << std::endl;

                                while (pointAvailable == true)
                                {
                                    std::cout << "Waiting for holding point free " << std::endl;

                                    pointAvailable = checkPointAvailable("holdingpoint");
                                }
                                std::cout << "Holding Point Available " << std::endl;
                                pointAvailable = false;
                                taskID = goalDestination + "holdingpoint";
                                threadParam::MSMutex.lock();
                                MessagesFunc::dispatchAGV(taskID, tugID);
                                threadParam::MSMutex.unlock();
                                std::cout << "|==========================================================|" << std::endl;
                                cout << "Going to holding point " << jobID << endl;
                                std::cout << "|==========================================================|" << std::endl;
                                robotStatus = "";
                                while (robotStatus != "At_Destination")
                                {
                                    threadParam::MSMutex.lock();
                                    robotStatus = MessagesFunc::checkRobotStatus(tugID);
                                    threadParam::MSMutex.unlock();
                                    robotStatusUpdate(taskID, jobID);
                                    cout << taskID << endl;
                                    rfidCheck(conveyorID, departmentID, "Scheduled");
                                    usleep(1 * 3000000);
                                }
                                usleep(3000000);
                                std::cout << "|==========================================================|" << std::endl;
                                cout << "Reached holding point " << jobID << endl;
                                std::cout << "|==========================================================|" << std::endl;
                                threadParam::MSMutex.lock();
                                std::string bayID;
                                bayID = MessagesFunc::BayAvailable(goalDestination, bayLobbyAvailable);
                                threadParam::MSMutex.unlock();
                                if ((bayID != "") && (bayID != "Occupied") && (bayID != "Failure") && (bayID == "Not Occupied"))
                                {
                                    taskID = goalDestination + bayLobbyAvailable;
                                    threadParam::MSMutex.lock();
                                    MessagesFunc::dispatchAGV(taskID, tugID);
                                    threadParam::MSMutex.unlock();
                                    std::cout << "|==========================================================|" << std::endl;
                                    cout << "Going to bay " << jobID << endl;
                                    std::cout << "|==========================================================|" << std::endl;
                                    robotStatus = "";
                                    while (robotStatus != "At_Destination")
                                    {
                                        threadParam::MSMutex.lock();
                                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                                        threadParam::MSMutex.unlock();
                                        robotStatusUpdate(taskID, jobID);
                                        cout << taskID << endl;
                                        rfidCheck(conveyorID, departmentID, "Scheduled");
                                        usleep(1 * 3000000);
                                    }
                                    usleep(3000000);
                                    std::cout << "|==========================================================|" << std::endl;
                                    cout << "Reached bay " << jobID << endl;
                                    std::cout << "|==========================================================|" << std::endl;
                                    threadParam::MSMutex.lock();
                                    MessagesFunc::turnONArrivalLight(goalDestination);
                                    taskID = "Dock";
                                    MessagesFunc::dispatchAGV(taskID, tugID);
                                    threadParam::MSMutex.unlock();
                                    std::cout << "|==========================================================|" << std::endl;
                                    cout << "Going to Dock " << jobID << endl;
                                    std::cout << "|==========================================================|" << std::endl;
                                    robotStatus = "";
                                    while (robotStatus != "At_Destination")
                                    {
                                        threadParam::MSMutex.lock();
                                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                                        threadParam::MSMutex.unlock();
                                        robotStatusUpdate(taskID, jobID);
                                        cout << taskID << endl;
                                        rfidCheck(conveyorID, departmentID, "Scheduled");
                                        usleep(1 * 3000000);
                                    }
                                    std::cout << "|==========================================================|" << std::endl;
                                    cout << "Reached Dock " << jobID << endl;
                                    std::cout << "|==========================================================|" << std::endl;
                                    threadParam::dbMutex.lock();
                                    updateJobCompleted(jobID);
                                    threadParam::dbMutex.unlock();
                                }
                                else
                                {
                                    cout << "Bay occupied alert raised " << bayID << endl;
                                }
                            }
                            else
                            {
                                cout << "Bay occupied alert raised " << endl;
                            }
                        }
                        else
                        {
                            cout << "Bay occupied alert raised " << endl;
                        }
                    }
                    else
                    {
                        cout << "goal is not available for the conveyor " << endl;
                    }
                }
                else
                {
                    cout << "agv not available" << endl;
                    threadParam::dbMutex.lock();
                    updateJobBuffer(jobID);
                    threadParam::dbMutex.unlock();
                }
            }
            else
            {

                cout << "adhoc jobs" << endl;
                threadParam::dbMutex.lock();
                updateJobAdHoc(jobID);
                threadParam::dbMutex.unlock();
            }
        }
        threadParam::mapMutex.lock();
        threadParam::robotStatusMap.erase(jobID);
        threadParam::mapMutex.unlock();
    }

    static void returnTrolleyFlow(const int jobID, const string lobbyID)
    {
        if (jobID != 0)
        {
            threadParam::MSMutex.lock();
            MessagesFunc::turnONWarningLight("WL1", 1);
            threadParam::MSMutex.unlock();
            std::cout << "|==========================================================|" << std::endl;
            cout << "Schedule Window Closed Check " << jobID << endl;
            std::cout << "|==========================================================|" << std::endl;
            cout << "Schedule Window Closed Check " << jobID << endl;
            const std::string departmentID = "Return";
            string SW = scheduleWindowClosed(departmentID);
            ;
            std::string conveyorID = "";
            cout << "Schedule Window Closed Check " << jobID << " " << SW << endl;

            if (SW == "closed")
            {
                std::cout << "|==========================================================|" << std::endl;
                cout << "AGV Available Check " << jobID << endl;
                std::cout << "|==========================================================|" << std::endl;
                threadParam::MSMutex.lock();
                string tugID = MessagesFunc::agvAvailable(departmentID, 1);
                if (tugID == "Unavailable")
                {
                    tugID = MessagesFunc::agvAvailable(departmentID, 2);
                }
                threadParam::MSMutex.unlock();
                if ((tugID != "") && (tugID != "Unavailable") && (tugID != "Failure"))
                {
                    string goalDestination, taskID, robotStatus;
                    cout << "AGV Available" << endl
                         << endl;
                    threadParam::dbMutex.lock();
                    updateJobActive(jobID, tugID);
                    threadParam::dbMutex.unlock();
                    taskID = lobbyID + "LiftWaitPoint";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Going to " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    threadParam::MSMutex.lock();
                    MessagesFunc::turnONWarningLight("WL1", 0);
                    threadParam::MSMutex.unlock();
                    
                    taskID = lobbyID + "HodingPoint";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    taskID = lobbyID + "Bay";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    // getRFIDDestination() (Send Query to get robot status) RFID Interface (TugID)
                    threadParam::MSMutex.lock();
                    std::string emptyConveyor = MessagesFunc::ConvAvailable("Kitchen"); // (Send Query to get "Not Occupied" conveyor) Conveyor Interface (DeptID)
                    threadParam::MSMutex.unlock();
                    if (emptyConveyor != "" && emptyConveyor != "Occupied" && emptyConveyor != "Failure") //(Send Query to get "KithcenConv1" conveyor) Conveyor Interface (DeptID)
                    {
                        int cdCheckCount = 0;
                        threadParam::MSMutex.lock();
                        string convDirection = MessagesFunc::ConvReverseQuery(emptyConveyor);
                        threadParam::MSMutex.unlock();
                        string convStop = "";
                        if (convDirection != "Backward")
                        {
                            cout << "Conveyor is not reversed" << endl;
                            if (convStop != "Stop")
                            {
                                while (convStop != "Stop")
                                {
                                    threadParam::MSMutex.lock();
                                    MessagesFunc::ConvStop(emptyConveyor);
                                    convStop = MessagesFunc::ConvStopQuery(emptyConveyor);
                                    threadParam::MSMutex.unlock();
                                    cdCheckCount++;
                                    if (cdCheckCount > 4)
                                    {
                                        break;
                                    }
                                }
                                if (convStop != "Stop")
                                {
                                    cout << "Conveyor cannot be stopped" << endl;
                                    return;
                                }
                                else
                                {
                                    cdCheckCount = 0;
                                    while (convDirection != "Backward")
                                    {
                                        threadParam::MSMutex.lock();
                                        MessagesFunc::ConvReverse(emptyConveyor);
                                        convDirection = MessagesFunc::ConvReverseQuery(emptyConveyor);
                                        threadParam::MSMutex.unlock();
                                        cdCheckCount++;
                                        if (cdCheckCount > 4)
                                        {
                                            break;
                                        }
                                    }
                                    if (convDirection != "Backward")
                                    {
                                        cout << "Conveyor cannot be reversed" << endl;
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        cout << "Conveyor not available" << endl;
                        return;
                    }
                    cout << "Conveyor is reversed" << endl;
                    taskID = lobbyID + "HodingPointR";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    taskID = lobbyID + "LiftWaitPointR";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    taskID = lobbyID + "ConveyorWaitPointR";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;

                    taskID = lobbyID + "ConveyorR";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;

                    taskID = lobbyID + "Dock";
                    threadParam::MSMutex.lock();
                    MessagesFunc::dispatchAGV(taskID, tugID);
                    threadParam::MSMutex.unlock();
                    robotStatus = "";
                    while (robotStatus != "At_Destination")
                    {
                        threadParam::MSMutex.lock();
                        robotStatus = MessagesFunc::checkRobotStatus(tugID);
                        threadParam::MSMutex.unlock();
                        robotStatusUpdate(taskID, jobID);
                        cout << taskID << " " << jobID << endl;
                        rfidCheck(conveyorID, departmentID, "Return");
                        usleep(1 * 3000000);
                    }
                    std::cout << "|==========================================================|" << std::endl;
                    cout << "Reached " << taskID << " " << jobID << endl;
                    std::cout << "|==========================================================|" << std::endl;
                    threadParam::dbMutex.lock();
                    updateJobCompleted(jobID); // UpdateDB as Completed
                    threadParam::dbMutex.unlock();
                }
                else
                {
                    cout << "agv not available" << endl;
                    threadParam::dbMutex.lock();
                    updateJobBuffer(jobID);
                    threadParam::dbMutex.unlock();
                }
            }
            else
            {

                cout << "adhoc jobs" << endl;
                threadParam::dbMutex.lock();
                updateJobAdHoc(jobID);
                threadParam::dbMutex.unlock();
            }
        }
    }
} // namespace MajorFlowFunc