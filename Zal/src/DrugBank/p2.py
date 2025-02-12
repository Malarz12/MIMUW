import xml.etree.ElementTree as ET
import pandas as pd
import os

# This script loads drug data from a DrugBank XML file by extracting each drug's DrugBank ID and its list of synonyms.
# The extracted data is organized into a pandas DataFrame with columns "DrugBank ID" and "Synonyms" and then saved to a JSON file.

def load_drugbank_data(file):
    tree = ET.parse(file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}
    drugs_data = []
    for drug in root.findall("db:drug", ns):
        drug_id = drug.find("db:drugbank-id", ns).text
        synonyms = drug.findall("db:synonyms/db:synonym", ns)
        synonym_list = [synonym.text for synonym in synonyms] if synonyms else []
        drugs_data.append([drug_id, synonym_list])
    df = pd.DataFrame(drugs_data, columns=["DrugBank ID", "Synonyms"])
    return df

if __name__ == "__main__":
    file = input("Podaj ścieżkę do pliku XML: ")
    df = load_drugbank_data(file)
    print(df)
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "p2_res.json")
    df.to_json(output_path, orient="records", indent=4)
