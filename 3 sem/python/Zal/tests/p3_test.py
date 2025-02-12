import sys
import os
import pytest
import pandas as pd

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/DrugBank")))
from p3 import load_products_data

@pytest.fixture
def sample_products_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
    <drug type="biotech">
        <drugbank-id primary="true">DB00001</drugbank-id>
        <products>
            <product>
                <name>Product1</name>
                <labeller>Manufacturer1</labeller>
                <ndc-product-code>12345</ndc-product-code>
                <dosage-form>Injection</dosage-form>
                <strength>10 mg</strength>
                <route>Intravenous</route>
                <country>US</country>
                <source>FDA</source>
            </product>
        </products>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00002</drugbank-id>
        <products>
            <product>
                <name>Product2</name>
                <labeller>Manufacturer2</labeller>
                <ndc-product-code>67890</ndc-product-code>
                <dosage-form>Tablet</dosage-form>
                <strength>20 mg</strength>
                <route>Oral</route>
                <country>Canada</country>
                <source>DPD</source>
            </product>
            <product>
                <name>Product3</name>
                <labeller>Manufacturer3</labeller>
                <ndc-product-code>11111</ndc-product-code>
                <dosage-form>Capsule</dosage-form>
                <strength>5 mg</strength>
                <route>Oral</route>
                <country>UK</country>
                <source>EMA</source>
            </product>
        </products>
    </drug>
</drugbank>'''
    file = tmp_path / "sample_products.xml"
    file.write_text(xml_content, encoding="utf-8")
    return file

def test_load_products_data(sample_products_xml):
    df = load_products_data(str(sample_products_xml))
    assert df.shape[0] == 3, "Powinny być 3 produkty w danych"
    row1 = df[df["DrugBank ID"] == "DB00001"].iloc[0]
    assert row1["Product Name"] == "Product1"
    assert row1["Manufacturer"] == "Manufacturer1"
    assert row1["NDC Product Code"] == "12345"
    assert row1["Dosage Form"] == "Injection"
    assert row1["Strength"] == "10 mg"
    assert row1["Route"] == "Intravenous"
    assert row1["Country"] == "US"
    assert row1["Regulatory Agency"] == "FDA"
    row2 = df[df["Product Name"] == "Product2"].iloc[0]
    assert row2["DrugBank ID"] == "DB00002"
    assert row2["Manufacturer"] == "Manufacturer2"
    assert row2["NDC Product Code"] == "67890"
    assert row2["Dosage Form"] == "Tablet"
    assert row2["Strength"] == "20 mg"
    assert row2["Route"] == "Oral"
    assert row2["Country"] == "Canada"
    assert row2["Regulatory Agency"] == "DPD"
    row3 = df[df["Product Name"] == "Product3"].iloc[0]
    assert row3["DrugBank ID"] == "DB00002"
    assert row3["Manufacturer"] == "Manufacturer3"
    assert row3["NDC Product Code"] == "11111"
    assert row3["Dosage Form"] == "Capsule"
    assert row3["Strength"] == "5 mg"
    assert row3["Route"] == "Oral"
    assert row3["Country"] == "UK"
    assert row3["Regulatory Agency"] == "EMA"