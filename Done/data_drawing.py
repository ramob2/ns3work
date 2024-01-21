import pandas as pd  
import numpy as np
import matplotlib.pyplot as plt

# # Read data from csv file
df_friis = pd.read_csv('friis_station.csv')
df_fixed = pd.read_csv('fixed_station.csv')
df_nakagami = pd.read_csv('nakagami_station.csv')
df_threelog = pd.read_csv('threelog_station.csv')
df_tworay = pd.read_csv('tworay_station.csv')
df_fsp = pd.read_csv('free_space_path_loss.csv')


x_daten = df_friis['distance']
x_daten2 = df_fixed['distance']
x_daten3 = df_nakagami['distance']
x_daten4 = df_threelog['distance']
x_daten5 = df_tworay['distance']
x_daten6 = df_fsp['distance']

y_daten = df_friis['troughput']
y_daten2 = df_fixed['troughput']
y_daten3 = df_nakagami['troughput']
y_daten4 = df_threelog['troughput']
y_daten5 = df_tworay['troughput']


plt.plot(x_daten, y_daten, linestyle='-', label='Friis')
plt.plot(x_daten2, y_daten2, marker = '.',linestyle='-', label='Fixed')
plt.plot(x_daten3, y_daten3,  linestyle='-', label='Nakagami')
plt.plot(x_daten4, y_daten4, linestyle='-', label='ThreeLog')
plt.plot(x_daten5, y_daten5, linestyle='-', label='TwoRay')

plt.xlabel('Distance [m]')
plt.ylabel('Data Layer Troughput[Mbit/s]')
plt.legend()
plt.xlim(1,260)
plt.ylim(30,80)
plt.show()




y_daten = df_friis['signal_mean']
y_daten2 = df_fixed['signal_mean']
y_daten3 = df_nakagami['signal_mean']
y_daten4 = df_threelog['signal_mean']
y_daten5 = df_tworay['signal_mean']
y_daten6 = df_fsp['signal_mean']
y_daten_noise = df_threelog['noise']

plt.plot(x_daten, y_daten, linestyle='-', label='Friis')
plt.plot(x_daten2, y_daten2, linestyle='-', label='Fixed')
#plt.plot(x_daten3, y_daten3,  linestyle='-', label='Nakagami')
plt.plot(x_daten4, y_daten4, linestyle='-', label='ThreeLog')
plt.plot(x_daten5, y_daten5, linestyle='-', label='TwoRay')
plt.plot(x_daten6, y_daten6, linestyle='-', label='Free Space Path Loss')
plt.plot(x_daten4, y_daten_noise, linestyle='-', label='Avg. Noise Lvl.')

plt.xlabel('Distance[m]')
plt.ylabel('RSSI[dBm]')
plt.legend()
plt.xlim(1,250)
plt.ylim(-94,-30)
plt.show()


df = pd.read_csv('friis_pre_experiment.csv')
x_daten = df['runtime']
y_daten = df['troughput']
something = [49]*len(x_daten)
plt.plot(x_daten, y_daten, linestyle='-', label='Troughput')
plt.plot(x_daten, something, linestyle=':', label='Reference Threshold')
plt.xlabel('Runtime [s]')
plt.legend()
plt.ylabel('Data Layer Troughput[Mbit/s]')
plt.xlim (0,95)

plt.show()
