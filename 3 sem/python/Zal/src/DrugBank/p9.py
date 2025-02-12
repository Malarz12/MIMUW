import os
import xml.etree.ElementTree as ET
import pandas as pd
import matplotlib.pyplot as plt

# This script reads a DrugBank XML file,
# counts the number of drugs in each status category (Approved, Withdrawn, Experimental/Investigational, and Vet Approved),
# creates a DataFrame with the results, and generates a pie chart showing the percentage distribution of these statuses.

file = input("Podaj ścieżkę do pliku XML: ")
tree = ET.parse(file)
root = tree.getroot()

ns = {"db": "http://www.drugbank.ca"}

approved_count = 0
withdrawn_count = 0
experimental_count = 0
vet_approved_count = 0

for drug in root.findall("db:drug", ns):
    groups_elem = drug.find("db:groups", ns)
    groups = []
    if groups_elem is not None:
        groups = [grp.text.strip().lower() for grp in groups_elem.findall("db:group", ns)]
    if "approved" in groups:
        approved_count += 1
    if "withdrawn" in groups:
        withdrawn_count += 1
    if "experimental" in groups or "investigational" in groups:
        experimental_count += 1
    if "vet_approved" in groups:
        vet_approved_count += 1

data = {
    "Category": [
        "Approved",
        "Withdrawn",
        "Experimental/Investigational",
        "Vet Approved"
    ],
    "Count": [approved_count, withdrawn_count, experimental_count, vet_approved_count]
}

df = pd.DataFrame(data)
print("Drug status DataFrame:")
print(df)

plt.figure(figsize=(8, 8))
plt.pie(df["Count"], labels=df["Category"], autopct='%1.1f%%', startangle=90)
plt.title("Percentage Distribution of Drug Statuses")
plt.axis("equal")

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)
output_path = os.path.join(output_dir, "p9_res.png")
plt.savefig(output_path, format="png", bbox_inches="tight", dpi=300)
plt.show()
