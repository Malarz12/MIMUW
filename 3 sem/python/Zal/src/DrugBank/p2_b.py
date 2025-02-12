import json
import networkx as nx
import matplotlib.pyplot as plt
import os

"""
This script loads drug synonym data from a JSON file, constructs a complete graph of synonyms for a specified DrugBank ID,
 and visualizes the graph using NetworkX and Matplotlib.
The resulting graph is saved as a PNG file.
"""

def load_data_from_json(file):
    with open(file, 'r') as f:
        data = json.load(f)
    return data

def create_synonym_graph(data, drugbank_id):
    synonyms = None
    for drug in data:
        if drug["DrugBank ID"] == drugbank_id:
            synonyms = drug["Synonyms"]
            break
    if synonyms is None:
        print(f"No data for DrugBank ID {drugbank_id}")
        return
    G = nx.Graph()
    for synonym in synonyms:
        G.add_node(synonym)
    for i in range(len(synonyms)):
        for j in range(i + 1, len(synonyms)):
            G.add_edge(synonyms[i], synonyms[j])
    pos = nx.spring_layout(G, k=0.5, iterations=20)
    plt.figure(figsize=(12, 10))
    nx.draw(G, pos, with_labels=True, node_size=3000, node_color="lightblue",
            font_size=10, font_weight="bold", edge_color="gray", alpha=0.7)
    plt.title(f"Synonym Graph for DrugBank ID {drugbank_id}")
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "p2_graph.png")
    plt.savefig(output_path, format="PNG", dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    file = input("Podaj ścieżkę do pliku JSON: ")
    data = load_data_from_json(file)
    drugbank_id = "DB00001"
    create_synonym_graph(data, drugbank_id)
