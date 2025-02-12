import os
import xml.etree.ElementTree as ET
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# This script reads gene-drug interactions from an XML file,
# builds a graph representing the relationships between a given gene, drugs
# and pharmaceutical products, and visualizes the graph using NetworkX and Matplotlib.

gene_input = input("Enter gene ID (e.g., EGFR): ").strip()
if not gene_input:
    print("Gene ID not provided. Exiting program.")
    exit(1)
gene_input = gene_input.upper()

xml_file = input("Podaj ścieżkę do pliku XML: ").strip()
tree = ET.parse(xml_file)
root = tree.getroot()
ns = {"db": "http://www.drugbank.ca"}

drugs_interacting = set()
drug_to_products = {}

for drug in root.findall("db:drug", ns):
    name_elem = drug.find("db:name", ns)
    if name_elem is None:
        continue
    drug_name = name_elem.text.strip()
    targets_elem = drug.find("db:targets", ns)
    found = False
    if targets_elem is not None:
        for target in targets_elem.findall("db:target", ns):
            polypep = target.find("db:polypeptide", ns)
            if polypep is None:
                continue
            gene_elem = polypep.find("db:gene-name", ns)
            if gene_elem is not None:
                target_gene = gene_elem.text.strip().upper()
                if target_gene == gene_input:
                    found = True
                    break
    if found:
        drugs_interacting.add(drug_name)
        products_elem = drug.find("db:products", ns)
        if products_elem is not None:
            for product in products_elem.findall("db:product", ns):
                prod_name_elem = product.find("db:name", ns)
                if prod_name_elem is not None:
                    prod_name = prod_name_elem.text.strip()
                    if drug_name in drug_to_products:
                        drug_to_products[drug_name].add(prod_name)
                    else:
                        drug_to_products[drug_name] = {prod_name}

if not drugs_interacting:
    print(f"No drugs found containing gene '{gene_input}'.")
    exit(0)

G = nx.Graph()
G.add_node(gene_input, layer='gene')
for drug in drugs_interacting:
    G.add_node(drug, layer='drug')
    G.add_edge(gene_input, drug)
for drug, products in drug_to_products.items():
    if drug in drugs_interacting:
        for product in products:
            G.add_node(product, layer='product')
            G.add_edge(drug, product)

pos = nx.spring_layout(G, seed=42)
plt.figure(figsize=(12, 8))
node_colors = []
for node, attr in G.nodes(data=True):
    if attr['layer'] == 'gene':
        node_colors.append('purple')
    elif attr['layer'] == 'drug':
        node_colors.append('orange')
    elif attr['layer'] == 'product':
        node_colors.append('teal')

nx.draw(G, pos, with_labels=True, node_color=node_colors, node_size=1500,
        font_size=12, font_weight='bold', edge_color='gray', width=2)
plt.title(f"Interactions of gene {gene_input} with drugs and pharmaceutical products", fontsize=16)
plt.axis("off")
plt.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.05)
gene_patch = mpatches.Patch(color='purple', label='Gene')
drug_patch = mpatches.Patch(color='orange', label='Drug')
product_patch = mpatches.Patch(color='teal', label='Product')
plt.legend(handles=[gene_patch, drug_patch, product_patch], loc='upper left')

current_dir = os.path.abspath(os.path.dirname(__file__))
if os.path.basename(current_dir) == "src":
    project_root = os.path.abspath(os.path.join(current_dir, ".."))
else:
    project_root = current_dir
output_dir = os.path.join(project_root, "results")
os.makedirs(output_dir, exist_ok=True)
output_file = os.path.join(output_dir, "p11_res.png")
plt.savefig(output_file, format="PNG", dpi=300, bbox_inches='tight')
plt.show()
print(f"Graph saved as: {output_file}")
