import os
import random
import copy
import xml.etree.ElementTree as ET
from pathlib import Path

"""
This script parses an existing DrugBank XML file, builds pools of second-level elements from each drug,
and then generates a user-specified number of new drug entries by randomly selecting elements from these pools.
Each new drug receives a new DrugBank ID (in the format 'DBxxxxx'), and the augmented XML is saved to an output file.
"""

def get_namespace(root):
    return root.tag.split("}")[0].strip("{") if root.tag.startswith("{") else ""

def build_pools(drugs, ns):
    pools = {}
    for drug in drugs:
        for child in drug:
            pools.setdefault(child.tag, []).append(copy.deepcopy(child))
    return pools

def create_new_drug(new_id, pools, ns):
    id_tag = f"{{{ns}}}drugbank-id" if ns else "drugbank-id"
    drug = ET.Element(f"{{{ns}}}drug") if ns else ET.Element("drug")
    primary = ET.Element(id_tag, {"primary": "true"})
    primary.text = new_id
    drug.append(primary)
    for tag, elems in pools.items():
        if tag == id_tag:
            continue
        drug.append(copy.deepcopy(random.choice(elems)))
    return drug

xml_file = input("Podaj ścieżkę do pliku XML: ").strip()
original_file = Path(xml_file)

tree = ET.parse(original_file)
root = tree.getroot()
ns = get_namespace(root)
drugs = root.findall(f"{{{ns}}}drug") if ns else root.findall("drug")
pools = build_pools(drugs, ns)

id_tag = f"{{{ns}}}drugbank-id" if ns else "drugbank-id"
max_id = 0
for drug in drugs:
    for elem in drug.findall(id_tag):
        if elem.get("primary") == "true" and elem.text:
            try:
                num = int(elem.text.strip()[2:])
                max_id = max(max_id, num)
            except:
                pass

try:
    num_generated = int(input("Enter the number of new drugs to generate: "))
except:
    num_generated = 300

for i in range(num_generated):
    new_id = "DB" + str(max_id + i + 1).zfill(5)
    new_drug = create_new_drug(new_id, pools, ns)
    root.append(new_drug)

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)
output_file = os.path.join(output_dir, "drugbank_partial_and_generated.xml")
tree.write(output_file, encoding="utf-8", xml_declaration=True)
print(f"Generated database saved to: {output_file}")