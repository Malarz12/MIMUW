import os
import xml.etree.ElementTree as ET
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

# This FastAPI application provides a POST endpoint that accepts a drug ID and returns information about the drug.
# The drug information is read from an XML file during server startup and stored in a dictionary.
# The drug data includes the drug's name and the number of pathways associated with it.

app = FastAPI()
current_dir = os.path.dirname(os.path.abspath(__file__))
xml_file = os.path.join(current_dir, "drugbank_partial.xml")

try:
    tree = ET.parse(xml_file)
except FileNotFoundError:
    raise FileNotFoundError(f"XML file not found at: {xml_file}")

root = tree.getroot()
ns = {"db": "http://www.drugbank.ca"}

drug_data = {}
for drug in root.findall("db:drug", ns):
    primary_id_elem = None
    for elem in drug.findall("db:drugbank-id", ns):
        if elem.get("primary") == "true":
            primary_id_elem = elem
            break
    if primary_id_elem is None or primary_id_elem.text is None:
        continue
    drug_id = primary_id_elem.text.strip()

    name_elem = drug.find("db:name", ns)
    drug_name = name_elem.text.strip() if name_elem is not None else "No data"

    pathways_elem = drug.find("db:pathways", ns)
    count = len(pathways_elem.findall("db:pathway", ns)) if pathways_elem is not None else 0

    drug_data[drug_id] = {"drug_name": drug_name, "num_pathways": count}

class DrugRequest(BaseModel):
    drug_id: str

@app.post("/drug")
async def get_drug_info(request: DrugRequest):
    drug_id = request.drug_id.strip()
    if drug_id not in drug_data:
        raise HTTPException(status_code=404, detail="Drug with the given ID not found.")
    return drug_data[drug_id]
