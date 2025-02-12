import os
import xml.etree.ElementTree as ET
import json

# This script loads drug interaction data for a given drug (specified by its DrugBank ID)
# from an XML file and returns a list of lists [Interacting DrugBank ID, Interacting Drug Name, Interaction Description].
# If the drug is found but has no interactions, an empty list is returned. If the drug is not found, the function returns None.
# When executed as a script, it prompts the user for a DrugBank ID, prints the interactions, and saves the result to a JSON file.

def load_drug_interactions_data(file, drugbank_id_input):
    tree = ET.parse(file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}
    interactions_data = []
    drug_found = False
    for drug in root.findall("db:drug", ns):
        drug_id_elem = drug.find("db:drugbank-id", ns)
        if drug_id_elem is None:
            continue
        current_drug_id = drug_id_elem.text.strip()
        if current_drug_id == drugbank_id_input:
            drug_found = True
            drug_interactions = drug.find("db:drug-interactions", ns)
            if drug_interactions is not None:
                for interaction in drug_interactions.findall("db:drug-interaction", ns):
                    interacting_id_elem = interaction.find("db:drugbank-id", ns)
                    interacting_drug_id = interacting_id_elem.text.strip() if interacting_id_elem is not None else "No data"
                    interacting_name_elem = interaction.find("db:name", ns)
                    interacting_drug_name = interacting_name_elem.text.strip() if interacting_name_elem is not None else "No data"
                    description_elem = interaction.find("db:description", ns)
                    description = description_elem.text.strip() if description_elem is not None else "No data"
                    interactions_data.append([interacting_drug_id, interacting_drug_name, description])
            break
    if not drug_found:
        return None
    return interactions_data

if __name__ == "__main__":
    drugbank_id_input = input("Enter the DrugBank ID of the drug: ").strip()
    xml_file = input("Podaj ścieżkę do pliku XML: ").strip()
    result = load_drug_interactions_data(xml_file, drugbank_id_input)
    if result is None:
        print(f"Drug with ID {drugbank_id_input} was not found in the XML file.")
    elif not result:
        print(f"Drug with ID {drugbank_id_input} has no interaction information.")
    else:
        print("Interactions for drug", drugbank_id_input, ":")
        print(result)
        current_dir = os.path.abspath(os.path.dirname(__file__))
        if os.path.basename(current_dir) == "src":
            project_root = os.path.abspath(os.path.join(current_dir, ".."))
        else:
            project_root = current_dir
        output_dir = os.path.join(project_root, "results")
        os.makedirs(output_dir, exist_ok=True)
        output_file = os.path.join(output_dir, "p10_res.json")
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(result, f, indent=4, ensure_ascii=False)
        print(f"Result saved to file {output_file}.")
