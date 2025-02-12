import sys
import os
import pytest
import pandas as pd
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/DrugBank")))
from p2 import load_drugbank_data

@pytest.fixture
def sample_xml(tmp_path):
    xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<drugbank xmlns="http://www.drugbank.ca">
    <drug type="biotech">
        <drugbank-id primary="true">DB00001</drugbank-id>
        <synonyms>
            <synonym>Syn1</synonym>
            <synonym>Syn2</synonym>
        </synonyms>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00002</drugbank-id>
        <synonyms>
            <synonym>SynA</synonym>
        </synonyms>
    </drug>
</drugbank>'''
    file = tmp_path / "sample.xml"
    file.write_text(xml_content, encoding="utf-8")
    return file

def test_load_drugbank_data(sample_xml):
    df = load_drugbank_data(str(sample_xml))
    assert df.shape[0] == 2

    row1 = df.iloc[0]
    assert row1["DrugBank ID"] == "DB00001"
    assert row1["Synonyms"] == ["Syn1", "Syn2"]

    row2 = df.iloc[1]
    assert row2["DrugBank ID"] == "DB00002"
    assert row2["Synonyms"] == ["SynA"]
