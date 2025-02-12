import xml.etree.ElementTree as ET
import pandas as pd
import os

# This script loads pathway data from a DrugBank XML file and returns a DataFrame with columns:
# "Pathway Name", "Category", "SMPDB ID". It then prints the total number of unique pathways and saves the DataFrame to a JSON file.

def load_pathways_data(file):
    tree = ET.parse(file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}

    pathways_data = []
    for drug in root.findall("db:drug", ns):
        pathways_elem = drug.find("db:pathways", ns)
        if pathways_elem is not None:
            for pathway in pathways_elem.findall("db:pathway", ns):
                name_elem = pathway.find("db:name", ns)
                pathway_name = name_elem.text.strip() if name_elem is not None else "No data"
                category_elem = pathway.find("db:category", ns)
                category = category_elem.text.strip() if category_elem is not None else "No data"
                smpdb_elem = pathway.find("db:smpdb-id", ns)
                smpdb_id = smpdb_elem.text.strip() if smpdb_elem is not None else "No data"
                pathways_data.append([pathway_name, category, smpdb_id])
    df = pd.DataFrame(pathways_data, columns=["Pathway Name", "Category", "SMPDB ID"])
    return df

if __name__ == "__main__":
    file = input("Podaj ścieżkę do pliku XML: ")
    df_pathways = load_pathways_data(file)
    total_unique_pathways = df_pathways["Pathway Name"].nunique()
    print("Total number of unique pathways:", total_unique_pathways)
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "p4_res.json")
    df_pathways.to_json(output_path, orient="records", indent=4, force_ascii=False)
