import json
import matplotlib.pyplot as plt

with open('celeritas.out.json', 'r') as infile:
    data = json.load(infile)

step_counts = data['result']['optical-step-diagnostic']['steps'][0]

x = [float(i+1) for i in range(len(step_counts))]

plt.bar(x, step_counts, log=True)
plt.savefig("steps.png")
plt.clf()

times = data['result']['time']['events'][0]
max_time = 0
for time in times:
    max_time = max(time, max_time)

print("Maximum time: {}s".format(max_time))
