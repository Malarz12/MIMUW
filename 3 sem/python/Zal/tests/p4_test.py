import os
import sys
import pytest
import pandas as pd

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/DrugBank")))
from p4 import load_pathways_data

@pytest.fixture
def sample_pathways_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
    <drug type="biotech">
        <drugbank-id primary="true">DB00001</drugbank-id>
        <pathways>
            <pathway>
                <name>Pathway1</name>
                <category>metabolic</category>
                <smpdb-id>SMP000001</smpdb-id>
            </pathway>
            <pathway>
                <name>Pathway2</name>
                <category>signaling</category>
                <smpdb-id>SMP000002</smpdb-id>
            </pathway>
        </pathways>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00002</drugbank-id>
        <pathways>
            <pathway>
                <name>Pathway1</name>
                <category>metabolic</category>
                <smpdb-id>SMP000001</smpdb-id>
            </pathway>
        </pathways>
    </drug>
</drugbank>'''
    file = tmp_path / "sample_pathways.xml"
    file.write_text(xml_content, encoding="utf-8")
    return file

def test_load_pathways_data(sample_pathways_xml):
    df = load_pathways_data(str(sample_pathways_xml))
    assert df.shape[0] == 3, "Powinny być 3 rekordy szlaków"
    expected_columns = ["Pathway Name", "Category", "SMPDB ID"]
    assert list(df.columns) == expected_columns, "Kolumny DataFrame nie są zgodne"
    unique_count = df["Pathway Name"].nunique()
    assert unique_count == 2, "Powinny być 2 unikalne nazwy szlaków (Pathway1, Pathway2)"
    row1 = df[df["Pathway Name"] == "Pathway1"].iloc[0]
    assert row1["Category"] == "metabolic"
    assert row1["SMPDB ID"] == "SMP000001"
    row2 = df[df["Pathway Name"] == "Pathway2"].iloc[0]
    assert row2["Category"] == "signaling"
    assert row2["SMPDB ID"] == "SMP000002"
