import requests
import json
import urllib3

urllib3.disable_warnings()

gophish_base_url = "https://127.0.0.1:3333"
api_key = "" # Replace with your API KEY

headers = {
    "Content-Type": "application/json",
    "Authorization": f"Bearer {api_key}"
}

html_files = [
    {"name": "Ameli", "path": "templates/ameli.html"},
    {"name": "GTA", "path": "templates/GTA.html"},
    {"name": "Valorant", "path": "templates/valorant.html"}
]

for html_file in html_files:
    with open(html_file["path"], "r") as file:
        html_content = file.read()

    template_data = {
        "name": html_file["name"],
        "subject": f"New {html_file['name']} mail",
        "text": "Phising mail",
        "html": html_content
    }

    response = requests.post(
        f"{gophish_base_url}/api/templates",
        headers=headers,
        json=template_data,
        verify=False
    )

    if response.status_code == 201:
        print(f"Email template '{template_data['name']}' added successfully.")
    else:
        print(f"Error adding template '{template_data['name']}':")
        print(f"HTTP status code: {response.status_code}")
        print("Server response:", response.text)
