import os
import json
import pandas as pd
import matplotlib.pyplot as plt

# This script loads drug target data from a JSON file,
# extracts the cellular locations of each target, aggregates the occurrence of each location,
# and creates a pie chart showing the percentage distribution of targets across different cellular locations.
# The resulting chart is saved as a PNG file.

json_file = input("Podaj ścieżkę do pliku JSON: ")
with open(json_file, encoding="utf-8") as f:
    all_drug_targets = json.load(f)

cellular_locations = []
for drug in all_drug_targets:
    for target in drug.get("Targets", []):
        loc = target.get("Cellular Location", "No data")
        cellular_locations.append(loc)

location_counts = pd.Series(cellular_locations).value_counts()

plt.figure(figsize=(8, 8))
plt.pie(location_counts, labels=location_counts.index, autopct='%1.1f%%', startangle=90)
plt.title("Percentage Distribution of Targets in Cellular Locations")
plt.axis("equal")

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)
output_path = os.path.join(output_dir, "p8_res.png")
plt.savefig(output_path, format="png", bbox_inches="tight", dpi=300)
plt.show()