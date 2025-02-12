import os
import sys
import pytest
import pandas as pd

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/DrugBank")))
from p10 import load_drug_interactions_data

@pytest.fixture
def sample_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
  <drug>
    <drugbank-id>DB00001</drugbank-id>
    <name>Lepirudin</name>
    <drug-interactions>
      <drug-interaction>
        <drugbank-id>DB00002</drugbank-id>
        <name>Cetuximab</name>
        <description>Cetuximab may increase the risk of bleeding when combined with Lepirudin.</description>
      </drug-interaction>
      <drug-interaction>
        <drugbank-id>DB00003</drugbank-id>
        <name>Other Drug</name>
        <description>Other interaction description.</description>
      </drug-interaction>
    </drug-interactions>
  </drug>
  <drug>
    <drugbank-id>DB00004</drugbank-id>
    <name>Another Drug</name>
  </drug>
</drugbank>
'''
    file = tmp_path / "sample.xml"
    file.write_text(xml_content, encoding="utf-8")
    return str(file)

def test_load_interactions_data_found(sample_xml):
    result = load_drug_interactions_data(sample_xml, "DB00001")
    expected = [
        ["DB00002", "Cetuximab", "Cetuximab may increase the risk of bleeding when combined with Lepirudin."],
        ["DB00003", "Other Drug", "Other interaction description."]
    ]
    assert result == expected

def test_load_interactions_data_no_interactions(sample_xml):
    result = load_drug_interactions_data(sample_xml, "DB00004")
    assert result == []

def test_load_interactions_data_not_found(sample_xml):
    result = load_drug_interactions_data(sample_xml, "DB99999")
    assert result is None
