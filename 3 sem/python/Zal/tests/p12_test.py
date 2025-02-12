import os
import sys
import pytest
import pandas as pd

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../src/UniProt")))
from p12 import search_disease

@pytest.fixture
def sample_data():
    return {
        "results": [
            {
                "name": "HSD10 mitochondrial disease",
                "alternativeNames": [
                    "17-beta-hydroxysteroid dehydrogenase X deficiency",
                    "2-methyl-3-hydroxybutyryl-CoA dehydrogenase deficiency",
                    "3-hydroxyacyl-CoA dehydrogenase II deficiency"
                ]
            },
            {
                "name": "Disease X",
                "alternativeNames": ["Synonym 1", "Synonym 2"]
            }
        ]
    }

def test_search_by_main_name(sample_data):
    result = search_disease(sample_data, "HSD10 mitochondrial disease")
    assert result is not None
    assert result["name"] == "HSD10 mitochondrial disease"
    assert "alternativeNames" in result
    assert isinstance(result["alternativeNames"], list)

def test_search_by_synonym(sample_data):
    result = search_disease(sample_data, "2-methyl-3-hydroxybutyryl-CoA dehydrogenase deficiency")
    assert result is not None
    assert result["name"] == "HSD10 mitochondrial disease"

def test_search_not_found(sample_data):
    result = search_disease(sample_data, "Nonexistent disease name")
    assert result is None
