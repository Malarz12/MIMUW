import os
import sys
import pytest
import pandas as pd

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/DrugBank")))
from p7 import load_drug_targets_data

@pytest.fixture
def sample_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
    <drug>
        <drugbank-id>Lepirudin</drugbank-id>
        <name>Lepirudin</name>
        <targets>
            <target>
                <id>TARGET1</id>
                <polypeptide id="P12345" source="Swiss-Prot">
                    <name>Prothrombin</name>
                    <gene-name>F2</gene-name>
                    <external-identifiers>
                        <external-identifier>
                            <resource>GenAtlas</resource>
                            <identifier>GA1234</identifier>
                        </external-identifier>
                    </external-identifiers>
                    <chromosome-location>11</chromosome-location>
                    <cellular-location>Secreted</cellular-location>
                </polypeptide>
            </target>
        </targets>
    </drug>
</drugbank>
'''
    file = tmp_path / "sample.xml"
    file.write_text(xml_content, encoding="utf-8")
    return str(file)

def test_load_drug_targets_data(sample_xml):
    result = load_drug_targets_data(sample_xml)
    expected = [
        {
            "DrugBank ID Drug": "Lepirudin",
            "Drug Name": "Lepirudin",
            "Targets": [
                {
                    "DrugBank ID Target": "TARGET1",
                    "Source": "Swiss-Prot",
                    "External DB ID": "P12345",
                    "Polypeptide Name": "Prothrombin",
                    "Gene Name": "F2",
                    "GenAtlas ID": "GA1234",
                    "Chromosome": "11",
                    "Cellular Location": "Secreted"
                }
            ]
        }
    ]
    assert result == expected