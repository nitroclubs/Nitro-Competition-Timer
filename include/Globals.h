#pragma once
#ifndef Globals_h
#define Globals_h

#if DEBUG_MODE == 1
#define  debugPrint(x) Serial.print(x)
#define  debugPrintln(x) Serial.println(x)
#define  debugPrintf(x) Serial.printf(x)
#define  debugPrintf1(x,y) Serial.printf(x,y)
#define  debugPrintf2(x,y,z) Serial.printf(x,y,z)
#define  debugPrintf3(w,x,y,z) Serial.printf(w,x,y,z)
#define  debugPrintf4(v,w,x,y,z) Serial.printf(v,w,x,y,z)
#define  debugPrintf5(u,v,w,x,y,z) Serial.printf(u,v,w,x,y,z)
#define  debugPrintf6(t,u,v,w,x,y,z) Serial.printf(t,u,v,w,x,y,z)

#else
#define  debugPrint(x)
#define  debugPrintln(x)
#define  debugPrintf(x)
#define  debugPrintf1(x,y)
#define  debugPrintf2(x,y,z)
#define  debugPrintf3(w,x,y,z)
#define  debugPrintf4(v,w,x,y,z)
#define  debugPrintf5(u,v,w,x,y,z)
#define  debugPrintf6(t,u,v,w,x,y,z)
#endif

enum class CompetitionType { LINE_FOLLOWING, OBSTACLE_AVOIDING_LINE_FOLLOWING, MAZE_SOLVING, REMOTE_CONTROL };
enum class CompetitionState {NotStarted,InProgress,Finished};
// int passCheckEnableInMillis = 5000;



#endif //Globals_h