"""Shared helpers: GitHub ProjectV2 GraphQL access for issue risk fields."""
import json
import os
import sys
import time
import urllib.request

API = "https://api.github.com/graphql"
LEVELS = ["Lowest", "Low", "Medium", "High", "Highest"]  # index+1 == numeric value 1..5
LIKELIHOOD = ["Singular", "Uncommon", "Common", "Very Common", "Universal"]


def token():
    for name in ("AUTOBOT_API_KEY", "GITHUB_TOKEN", "GH_TOKEN"):
        if os.environ.get(name):
            return os.environ[name]
    sys.exit("no GitHub token in AUTOBOT_API_KEY/GITHUB_TOKEN/GH_TOKEN")


def gql(query, variables=None):
    req = urllib.request.Request(
        API,
        data=json.dumps({"query": query, "variables": variables or {}}).encode(),
        headers={"Authorization": "bearer " + token(), "Content-Type": "application/json"},
    )
    body = json.load(urllib.request.urlopen(req))
    if "errors" in body:
        raise RuntimeError(json.dumps(body["errors"]))
    return body["data"]


def project(owner, number):
    """Return {id, title, fields:{name:{id, options:{name:{id,description}}}}} for an org or user project."""
    q = """query($o:String!,$n:Int!){ %s(login:$o){ projectV2(number:$n){ id title
      fields(first:50){ nodes{
        ... on ProjectV2FieldCommon{ id name dataType }
        ... on ProjectV2SingleSelectField{ options{ id name description } } } } } } }"""
    for kind in ("organization", "user"):
        try:
            p = gql(q % kind, {"o": owner, "n": number})[kind]["projectV2"]
        except RuntimeError:
            continue
        if p:
            fields = {}
            for f in p["fields"]["nodes"]:
                if not f:
                    continue
                fields[f["name"]] = {
                    "id": f["id"],
                    "type": f["dataType"],
                    "options": {o["name"]: o for o in f.get("options", []) or []},
                }
            return {"id": p["id"], "title": p["title"], "fields": fields}
    sys.exit(f"project {owner}#{number} not found")


def items(project_id, field_names):
    """Yield (item_id, issue{number,url,title,repo,state}, {field_name: option_name}) for every project item."""
    q = """query($p:ID!,$after:String){ node(id:$p){ ... on ProjectV2{ items(first:100,after:$after){
      pageInfo{hasNextPage endCursor}
      nodes{ id content{ ... on Issue{ number url title state repository{nameWithOwner} }
                         ... on PullRequest{ number url title state repository{nameWithOwner} } }
        fieldValues(first:40){ nodes{ ... on ProjectV2ItemFieldSingleSelectValue{ name field{ ... on ProjectV2FieldCommon{ name } } } } } } } } } }"""
    after = None
    while True:
        page = gql(q, {"p": project_id, "after": after})["node"]["items"]
        for n in page["nodes"]:
            c = n["content"] or {}
            vals = {}
            for fv in n["fieldValues"]["nodes"]:
                if fv and fv.get("field", {}).get("name") in field_names:
                    vals[fv["field"]["name"]] = fv["name"]
            yield n["id"], c, vals
        if not page["pageInfo"]["hasNextPage"]:
            break
        after = page["pageInfo"]["endCursor"]


def set_option(project_id, item_id, field, option_name):
    opt = field["options"].get(option_name)
    if not opt:
        raise KeyError(f"option {option_name!r} not in field; have {list(field['options'])}")
    m = """mutation($p:ID!,$i:ID!,$f:ID!,$o:String!){ updateProjectV2ItemFieldValue(
        input:{projectId:$p,itemId:$i,fieldId:$f,value:{singleSelectOptionId:$o}}){ projectV2Item{ id } } }"""
    gql(m, {"p": project_id, "i": item_id, "f": field["id"], "o": opt["id"]})
    time.sleep(0.25)


def search_issue_numbers(query):
    """Issue numbers matching a GitHub search query (e.g. 'repo:nasa/fprime is:issue is:open label:bug')."""
    q = """query($q:String!,$after:String){ search(query:$q,type:ISSUE,first:100,after:$after){
      pageInfo{hasNextPage endCursor} nodes{ ... on Issue{ number repository{nameWithOwner} } } } }"""
    out, after = set(), None
    while True:
        page = gql(q, {"q": query, "after": after})["search"]
        out |= {(n["repository"]["nameWithOwner"], n["number"]) for n in page["nodes"] if n}
        if not page["pageInfo"]["hasNextPage"]:
            break
        after = page["pageInfo"]["endCursor"]
    return out
