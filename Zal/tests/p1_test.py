import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src/DrugBank')))

import pytest
import pandas as pd
from p1 import parse_drugbank_xml

@pytest.fixture
def sample_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
    <drug type="biotech">
        <drugbank-id primary="true">DB00001</drugbank-id>
        <name>Lepirudin</name>
        <description>Description 1</description>
        <dosage-form>Powder</dosage-form>
        <indication>Indication 1</indication>
        <mechanism-of-action>Mechanism 1</mechanism-of-action>
        <food-interactions>
            <food-interaction>Interaction 1</food-interaction>
            <food-interaction>Interaction 2</food-interaction>
        </food-interactions>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00002</drugbank-id>
        <name>Cetuximab</name>
        <description>Description 2</description>
        <dosage-form>Solution</dosage-form>
        <indication>Indication 2</indication>
        <mechanism-of-action>Mechanism 2</mechanism-of-action>
        <!-- No food-interactions section -->
    </drug>
</drugbank>'''
    file = tmp_path / "sample.xml"
    file.write_text(xml_content, encoding="utf-8")
    return file

def test_parse_drugbank_xml(sample_xml):
    df = parse_drugbank_xml(str(sample_xml))
    assert df.shape[0] == 2

    row1 = df.iloc[0]
    assert row1["DrugBank ID"] == "DB00001"
    assert row1["Name"] == "Lepirudin"
    assert row1["Type"] == "biotech"
    assert row1["Description"] == "Description 1"
    assert row1["Dosage Form"] == "Powder"
    assert row1["Indications"] == "Indication 1"
    assert row1["Mechanism of Action"] == "Mechanism 1"
    assert row1["Food Interactions"] == "Interaction 1; Interaction 2"

    row2 = df.iloc[1]
    assert row2["DrugBank ID"] == "DB00002"
    assert row2["Name"] == "Cetuximab"
    assert row2["Type"] == "small molecule"
    assert row2["Food Interactions"] == "No data"
