MANIFEST_FILENAME = 'idf_component.yml'
SLUG_BODY_REGEX = r'[a-zA-Z\d]+(?:(?:[_-](?![_-]+))|(?:[a-zA-Z\d]))*[a-zA-Z\d]+'
SLUG_REGEX = r'^{}$'.format(SLUG_BODY_REGEX)
FULL_SLUG_REGEX = r'^((?:{slug}/{slug})|(?:{slug}))$'.format(slug=SLUG_BODY_REGEX)
TAGS_REGEX = r'^[A-Za-z0-9\_\-]{3,32}$'
WEB_DEPENDENCY_REGEX = r'^((?:{slug}/{slug})|(?:{slug}))(.*)$'.format(slug=SLUG_BODY_REGEX)
COMMIT_ID_RE = r'[0-9a-f]{40}'
IF_IDF_VERSION_REGEX = r'^(?P<keyword>idf_version) *(?P<comparison>[\^=~<>!]+)(?P<spec>.+)$'
IF_TARGET_REGEX = r'^(?P<keyword>target) *(?P<comparison>!=|==|not in|in)(?P<versions>.+)$'
