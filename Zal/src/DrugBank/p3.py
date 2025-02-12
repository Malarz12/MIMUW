import xml.etree.ElementTree as ET
import pandas as pd
import os

# This script loads product data from a DrugBank XML file,
# extracts information about products associated with drugs, creates a DataFrame with columns for DrugBank ID,
# Product Name, Manufacturer, NDC Product Code, Dosage Form, Strength, Route, Country, and Regulatory Agency,
# and saves the DataFrame to a JSON file.

def load_products_data(file):
    tree = ET.parse(file)
    root = tree.getroot()
    ns = {"db": "http://www.drugbank.ca"}
    products_data = []
    for drug in root.findall("db:drug", ns):
        drug_id = drug.find("db:drugbank-id", ns).text
        products = drug.findall("db:products/db:product", ns)
        for product in products:
            product_info = {
                "DrugBank ID": drug_id,
                "Product Name": product.findtext("db:name", "No data", ns),
                "Manufacturer": product.findtext("db:labeller", "No data", ns),
                "NDC Product Code": product.findtext("db:ndc-product-code", "No data", ns),
                "Dosage Form": product.findtext("db:dosage-form", "No data", ns),
                "Strength": product.findtext("db:strength", "No data", ns),
                "Route": product.findtext("db:route", "No data", ns),
                "Country": product.findtext("db:country", "No data", ns),
                "Regulatory Agency": product.findtext("db:source", "No data", ns)
            }
            products_data.append(product_info)
    df = pd.DataFrame(products_data)
    return df

if __name__ == "__main__":
    file = input("Podaj ścieżkę do pliku XML: ")
    df_products = load_products_data(file)
    current_dir = os.path.abspath(os.path.dirname(__file__))
    if os.path.basename(current_dir) == "src":
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
    else:
        project_root = current_dir
    output_dir = os.path.join(project_root, "results")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "p3_res.json")
    df_products.to_json(output_path, orient="records", indent=4)
    print("Product data saved to 'results/p3_res.json'. Preview of the first 5 records:")
    print(df_products.head())
