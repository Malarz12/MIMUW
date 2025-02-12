import json
import os

# This script loads data from a JSON file and allows the user to search for a disease by its name or synonym.
# If the disease name or synonym is found, it will return the main name and all alternative names (synonyms).
# If a synonym is entered, it will also show the main name and the remaining synonyms. If no match is found, it will notify the user.

def search_disease(data, target_name):
    for record in data["results"]:
        main = record["name"].lower()
        alternatives = [alt.lower() for alt in record.get("alternativeNames", [])]
        if target_name.lower() == main or target_name.lower() in alternatives:
            return record
    return None

if __name__ == "__main__":
    json_file = input("Podaj ścieżkę do pliku JSON: ").strip()
    with open(json_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    target_name = input("Enter the disease name or synonym: ").strip()
    record = search_disease(data, target_name)

    if record is None:
        print("No disease found for the provided name.")
    else:
        main_name = record["name"]
        synonyms = record.get("alternativeNames", [])

        if target_name.lower() == main_name.lower():
            if synonyms:
                print(f"Synonyms for '{main_name}':")
                for synonym in synonyms:
                    print("- " + synonym)
            else:
                print(f"No synonyms found for '{main_name}'.")
        else:
            print("Main name:", main_name)
            other_synonyms = [syn for syn in synonyms if syn.lower() != target_name.lower()]
            if other_synonyms:
                print("Other synonyms:")
                for synonym in other_synonyms:
                    print("- " + synonym)
            else:
                print("No other synonyms found.")