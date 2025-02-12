import os
import xml.etree.ElementTree as ET
import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
import logging

# This script parses a DrugBank XML file to extract drug and pathway data,
# constructs a DataFrame summarizing signal, metabolic, and drug action pathways along with associated drugs,
# saves the DataFrame to a JSON file, and builds a bipartite graph showing the relationships between pathways and drugs.
# The graph is formatted with distinct colors for pathways and drugs, and saved as a PNG file.

logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['font.size'] = 12

file = input("Podaj ścieżkę do pliku XML: ")
tree = ET.parse(file)
root = tree.getroot()
ns = {"db": "http://www.drugbank.ca"}

drug_id_to_name = {}
for drug in root.findall("db:drug", ns):
    drug_id_elem = drug.find("db:drugbank-id", ns)
    if drug_id_elem is None:
        continue
    drug_id = drug_id_elem.text.strip()
    drug_name_elem = drug.find("db:name", ns)
    drug_name = drug_name_elem.text.strip() if drug_name_elem is not None else "No data"
    drug_id_to_name[drug_id] = drug_name

pathway_to_drugs = {}
for drug in root.findall("db:drug", ns):
    drug_id_elem = drug.find("db:drugbank-id", ns)
    if drug_id_elem is None:
        continue
    drug_id = drug_id_elem.text.strip()
    pathways_elem = drug.find("db:pathways", ns)
    if pathways_elem is None:
        continue
    for pathway in pathways_elem.findall("db:pathway", ns):
        name_elem = pathway.find("db:name", ns)
        pathway_name = name_elem.text.strip() if name_elem is not None else "No data"
        category_elem = pathway.find("db:category", ns)
        category = category_elem.text.strip().lower() if category_elem is not None else "no data"
        if ("signal" in category) or ("metabolic" in category) or ("drug_action" in category):
            key = (pathway_name, category)
            if key not in pathway_to_drugs:
                pathway_to_drugs[key] = set()
            pathway_to_drugs[key].add(drug_id)

pathways_data = []
for (pathway_name, category), drug_ids in pathway_to_drugs.items():
    pathways_data.append({
        "pathway_name": pathway_name,
        "category": category,
        "drugbank_ids": list(drug_ids),
        "number_of_drugs": len(drug_ids)
    })

df_pathways = pd.DataFrame(pathways_data)
logging.info("Created DataFrame with pathways.")
print("DataFrame with pathway information:")
print(df_pathways.head())

total_unique_pathways = df_pathways["pathway_name"].nunique()
logging.info(f"Total number of unique signal/metabolic pathways: {total_unique_pathways}")
print("Total number of unique signal/metabolic pathways:", total_unique_pathways)

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)
output_json_path = os.path.join(output_dir, "p5_res.json")
df_pathways.to_json(output_json_path, orient="records", indent=4, force_ascii=False)
logging.info(f"DataFrame saved to file {output_json_path}.")

B = nx.Graph()
for _, row in df_pathways.iterrows():
    pathway_name = row.get('pathway_name', 'Unknown Pathway')
    B.add_node(pathway_name, label='Pathway')
    for drug_id in row['drugbank_ids']:
        drug_name = drug_id_to_name.get(drug_id, drug_id)
        B.add_node(drug_name, label='Drug')
        B.add_edge(pathway_name, drug_name)

pos = nx.spring_layout(B, k=0.4, seed=42)
node_colors = []
for node, data in B.nodes(data=True):
    if data['label'] == 'Pathway':
        node_colors.append('lightcoral')
    else:
        node_colors.append('skyblue')

plt.figure(figsize=(20, 15))
nx.draw_networkx_nodes(B, pos, node_color=node_colors, node_size=3000, alpha=0.9, edgecolors='gray', linewidths=1.5)
nx.draw_networkx_edges(B, pos, width=2.0, alpha=0.6, edge_color='gray')
nx.draw_networkx_labels(B, pos, font_size=14, font_weight='bold', font_color='darkslategray')
plt.title("Bipartite Graph: Signal/Metabolic Pathways and Drugs", fontsize=24, color='darkred')
plt.axis('off')
plt.tight_layout()

graph_path = os.path.join(output_dir, "p5_graph.png")
plt.savefig(graph_path, format="PNG", dpi=300, bbox_inches='tight')
plt.close()
logging.info(f"Graph saved as PNG: {graph_path}")
