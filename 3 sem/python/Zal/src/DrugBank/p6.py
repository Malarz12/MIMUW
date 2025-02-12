import os
import xml.etree.ElementTree as ET
import pandas as pd
import matplotlib.pyplot as plt

# This script parses a DrugBank XML file to extract drug names and the number of pathways each drug is involved in,
# creates a DataFrame with this data, saves it to a JSON file, and generates a histogram of the number of pathways per drug.

file = input("Podaj ścieżkę do pliku XML: ")
tree = ET.parse(file)
root = tree.getroot()
ns = {"db": "http://www.drugbank.ca"}

drug_names = []
num_pathways = []

for drug in root.findall("db:drug", ns):
    name_elem = drug.find("db:name", ns)
    drug_name = name_elem.text.strip() if name_elem is not None else "No data"
    drug_names.append(drug_name)
    pathways_elem = drug.find("db:pathways", ns)
    if pathways_elem is not None:
        pathways = pathways_elem.findall("db:pathway", ns)
        count = len(pathways)
    else:
        count = 0
    num_pathways.append(count)

df = pd.DataFrame({
    "Drug Name": drug_names,
    "Number of Pathways": num_pathways
})
print("First 5 rows of the DataFrame:")
print(df.head())

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)

output_json_path = os.path.join(output_dir, "p6_res.json")
df.to_json(output_json_path, orient="records", indent=4, force_ascii=False)
print(f"Results saved to {output_json_path}.")

plt.figure(figsize=(10, 6))
bins = range(0, max(num_pathways) + 2)
plt.hist(num_pathways, bins=bins, edgecolor="black", align="left")
plt.xlabel("Number of pathways per drug")
plt.ylabel("Number of drugs")
plt.title("Histogram of the number of pathways per drug")
plt.xticks(range(0, max(num_pathways) + 1))
plt.tight_layout()

output_graph_path = os.path.join(output_dir, "p6_graph.png")
plt.savefig(output_graph_path, format="PNG", dpi=300, bbox_inches="tight")
plt.show()
