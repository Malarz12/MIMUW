import xml.etree.ElementTree as ET
import pandas as pd
import os

# This script parses a DrugBank XML file to extract drug information
# and returns a pandas DataFrame with columns for DrugBank ID,
# Name, Type, Description, Dosage Form, Indications, Mechanism of Action, and Food Interactions.
# The resulting DataFrame is then saved to a JSON file.
def parse_drugbank_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}
    drugs_data = []
    for drug in root.findall("db:drug", ns):
        drug_id = drug.find("db:drugbank-id", ns).text
        name = drug.find("db:name", ns).text
        drug_type = drug.get("type")
        description_elem = drug.find("db:description", ns)
        description = description_elem.text if description_elem is not None else "No data"
        dosage_form_elem = drug.find("db:dosage-form", ns)
        dosage_form = dosage_form_elem.text if dosage_form_elem is not None else "No data"
        indications_elem = drug.find("db:indication", ns)
        indications = indications_elem.text if indications_elem is not None else "No data"
        mechanism_elem = drug.find("db:mechanism-of-action", ns)
        mechanism = mechanism_elem.text if mechanism_elem is not None else "No data"
        food_interactions_elem = drug.find("db:food-interactions", ns)
        if food_interactions_elem is not None:
            food_interactions = "; ".join([fi.text for fi in food_interactions_elem.findall("db:food-interaction", ns)])
        else:
            food_interactions = "No data"
        drugs_data.append([
            drug_id, name, drug_type, description,
            dosage_form, indications, mechanism, food_interactions
        ])
    df = pd.DataFrame(
        drugs_data,
        columns=[
            "DrugBank ID", "Name", "Type", "Description",
            "Dosage Form", "Indications", "Mechanism of Action", "Food Interactions"
        ]
    )
    return df

if __name__ == "__main__":
    xml_file = input("Podaj ścieżkę do pliku XML: ")
    df = parse_drugbank_xml(xml_file)
    print(df.head())
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "p1_res.json")
    df.to_json(output_path, orient="records", indent=4, force_ascii=False)
    print(df.to_json(orient="records", indent=4, force_ascii=False))
