#!/bin/bash
cd /rsp01
/home/pi/.pyenv/versions/env_mission_main/bin/python /rsp01/run.py > /rsp01/mission.log 2>&1 &
