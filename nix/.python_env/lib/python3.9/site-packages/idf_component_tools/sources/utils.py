from idf_component_tools.sources.constants import DEFAULT_NAMESPACE


def normalized_name(name):
    if '/' not in name:
        name = '/'.join([DEFAULT_NAMESPACE, name])

    return name
