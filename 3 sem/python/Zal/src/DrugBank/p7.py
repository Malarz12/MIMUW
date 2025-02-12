import os
import xml.etree.ElementTree as ET
import json

# This script loads drug target data from an XML file,
# extracts information about drug targets including target ID, source, external DB ID,
# polypeptide name, gene name, GenAtlas ID, chromosome, and cellular location,
# and returns a list of dictionaries where each dictionary represents a drug along with its associated targets.
# The result is saved to a JSON file when the script is run.

def safe_text(elem, tag, ns, default="No data"):
    found = elem.find(tag, ns)
    return found.text.strip() if found is not None and found.text is not None else default

def load_drug_targets_data(file):
    tree = ET.parse(file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}
    all_drug_targets = []
    for drug in root.findall("db:drug", ns):
        drug_id = safe_text(drug, "db:drugbank-id", ns)
        drug_name = safe_text(drug, "db:name", ns)
        targets_list = []
        targets = drug.find("db:targets", ns)
        if targets is not None:
            for target in targets.findall("db:target", ns):
                target_id = safe_text(target, "db:id", ns)
                polypep = target.find("db:polypeptide", ns)
                if polypep is not None:
                    external_db_id = polypep.attrib.get("id", "No data")
                    source = polypep.attrib.get("source", "No data")
                    poly_name = safe_text(polypep, "db:name", ns)
                    gene_name = safe_text(polypep, "db:gene-name", ns)
                    genatlas = "No data"
                    ext_ids = polypep.find("db:external-identifiers", ns)
                    if ext_ids is not None:
                        for ext_id in ext_ids.findall("db:external-identifier", ns):
                            res = safe_text(ext_id, "db:resource", ns)
                            if res == "GenAtlas":
                                genatlas = safe_text(ext_id, "db:identifier", ns)
                                break
                    chromosome = safe_text(polypep, "db:chromosome-location", ns)
                    cellular_location = safe_text(polypep, "db:cellular-location", ns)
                else:
                    external_db_id = "No data"
                    source = "No data"
                    poly_name = "No data"
                    gene_name = "No data"
                    genatlas = "No data"
                    chromosome = "No data"
                    cellular_location = "No data"
                target_dict = {
                    "DrugBank ID Target": target_id,
                    "Source": source,
                    "External DB ID": external_db_id,
                    "Polypeptide Name": poly_name,
                    "Gene Name": gene_name,
                    "GenAtlas ID": genatlas,
                    "Chromosome": chromosome,
                    "Cellular Location": cellular_location
                }
                targets_list.append(target_dict)
        if targets_list:
            drug_entry = {
                "DrugBank ID Drug": drug_id,
                "Drug Name": drug_name,
                "Targets": targets_list
            }
            all_drug_targets.append(drug_entry)
    return all_drug_targets

if __name__ == "__main__":
    file = input("Podaj ścieżkę do pliku XML: ")
    data = load_drug_targets_data(file)
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, "p7_res.json")
    with open(output_file, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)
