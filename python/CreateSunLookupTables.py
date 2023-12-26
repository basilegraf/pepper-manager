#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 23 18:58:15 2023

@author: basile
"""


import datetime
from astral import LocationInfo
from astral.sun import sun
import astral
import itertools
import matplotlib.pyplot as plt
import numpy as np

import pandas as pd

 

city = LocationInfo("Neuchatel", "Switzerland", "Europe/Zurich", 47.0, 6.933333)

print((
    f"Information for {city.name}/{city.region}\n"
    f"Timezone: {city.timezone}\n"
    f"Latitude: {city.latitude:.02f}; Longitude: {city.longitude:.02f}\n"
))

 

 

#s = astral.sun.sun(city.observer, date=datetime.date(2009, 4, 22))
s = sun(city.observer, date=astral.now())#, tzinfo=city.tzinfo)
 

print((
    f'Dawn:    {s["dawn"].astimezone(city.tzinfo)}\n'
    f'Sunrise: {s["sunrise"].astimezone(city.tzinfo)}\n'
    f'Noon:    {s["noon"].astimezone(city.tzinfo)}\n'
    f'Sunset:  {s["sunset"].astimezone(city.tzinfo)}\n'
    f'Dusk:    {s["dusk"].astimezone(city.tzinfo)}\n'
))



d0=datetime.datetime.strptime("2024-1-01","%Y-%m-%d")
d1= datetime.datetime.strptime("2025-1-01", "%Y-%m-%d")
dates=[]
sunrises = []
sunsets = []
for days in range((d1-d0).days):
    dates.append((d0 + datetime.timedelta(days)).date())
    s = sun(city.observer, date = dates[-1])
    sunrises.append(s["sunrise"].astimezone(city.tzinfo).time())
    sunsets.append(s["sunset"].astimezone(city.tzinfo).time())
    
sunrisesHour = np.array([t.hour + t.minute / 60 for t in sunrises])
sunsetsHour = np.array([t.hour + t.minute / 60 for t in sunsets])
    

    
# Remove jump
(idxJump, ) = np.where(np.abs(np.diff(sunrisesHour)) > 0.5)
hoursOffset = np.ones(len(sunrisesHour))
hoursOffset[idxJump[0]+1:idxJump[1]+1] = 0.0

plt.figure()
plt.plot(sunrisesHour + hoursOffset)
plt.plot(sunsetsHour + hoursOffset)

# LUT function
def makeLUT(sunhour, name = 'arr'):
    (idxJump, ) = np.where(np.abs(np.diff(sunhour)) > 0.5)
    hoursOffset = np.ones(len(sunhour))
    hoursOffset[idxJump[0]+1:idxJump[1]+1] = 0.0
    sunhourCont = sunhour + hoursOffset
    sh400 = np.concatenate((sunhourCont[0:365], sunhourCont[0:(401-365)]))
    print(sh400.shape)
    plt.plot(sh400)
    sh10 = sh400[::40]
    print('float ' + name + '[] = {' + ', '.join(map(str,sh10)) + '};')
    plt.plot(np.linspace(0,401,11),sh10)
    return sh10, idxJump[0] + 1, idxJump[1]
    

plt.figure()
srLUT, idx1, idx2 = makeLUT(sunrisesHour, 'sunrisesHoursLUT')
ssLUT, _, _      = makeLUT(sunsetsHour, 'sunsetsHourLUT')
plt.grid('on')


def evalLUT(lut, day, idx1, idx2):
    day = day % 364
    i1 = int(int(day) / 40)
    i2 = i1 + 1
    l1 = lut[i1]
    l2 = lut[i2]
    val = 0.025 * ((40 * i2 - day) * l1 + (day - 40 * i1) * l2)
    if (day < idx1 or idx2 < day):
        val -= 1
    return val

checkSR = np.array([evalLUT(srLUT, d, idx1, idx2) for d in range(366)])
checkSS = np.array([evalLUT(ssLUT, d, idx1, idx2) for d in range(366)])
plt.figure()
plt.plot(sunrisesHour-checkSR)
plt.plot(sunsetsHour-checkSS)

    