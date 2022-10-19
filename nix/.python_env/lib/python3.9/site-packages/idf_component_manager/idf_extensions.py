import sys
import warnings

from idf_component_manager.utils import error, warn
from idf_component_tools.errors import FatalError

from .core import ComponentManager

try:
    from typing import Any, Dict, List
except ImportError:
    pass

SERVICE_PROFILE = [
    {
        'names': ['--service-profile'],
        'help': 'Profile for component registry to use. By default profile named "default" will be used.',
        'envvar': 'IDF_COMPONENT_SERVICE_PROFILE',
    },
]  # type: List[Dict[str, Any]]

NAMESPACE = [
    {
        'names': ['--namespace'],
        'help': 'Namespace for the component. Can be set in config file.',
        'envvar': 'IDF_COMPONENT_NAMESPACE',
    },
]  # type: List[Dict[str, Any]]

NAME = [
    {
        'names': ['--name'],
        'help': 'Component name.',
        'required': True,
    },
]  # type: List[Dict[str, Any]]

SERVICE_OPTIONS = SERVICE_PROFILE + NAMESPACE + NAME

LOCAL_MANIFEST_OPTIONS = [
    {
        'names': ['--component'],
        'default': 'main',
        'show_default': True,
        'help': 'Name of the component in the project.',
    },
]

VERSION_PARAMETER = [
    {
        'names': ['--version'],
        'help': 'Set version, if not defined in the manifest. Use "git" to get version from git tag. '
        "The command won\'t try uploading, if running not from a git tag.",
        'required': False,
    }
]


def action_extensions(base_actions, project_path):
    def callback(subcommand_name, ctx, args, **kwargs):
        try:
            with warnings.catch_warnings(record=True) as w:
                manager = ComponentManager(args.project_dir)
                getattr(manager, str(subcommand_name).replace('-', '_'))(kwargs)
                for warning in w:
                    warn(warning.message)
        except FatalError as e:
            error(e)
            sys.exit(e.exit_code)

    def global_callback(ctx, global_args, tasks):
        copy_tasks = list(tasks)
        for index, task in enumerate(copy_tasks):
            if task.name == 'fullclean':
                tasks.insert(index + 1, ctx.invoke(ctx.command.get_command(ctx, 'remove_managed_components')))

    return {
        'global_action_callbacks': [global_callback],
        'actions': {
            'create-manifest': {
                'callback': callback,
                'help': 'Create manifest for specified component.',
                'options': LOCAL_MANIFEST_OPTIONS,
            },
            'add-dependency': {
                'callback': callback,
                'help': (
                    'Add dependency to the manifest file. '
                    'For now we only support adding dependencies from the component registry.'),
                'arguments': [
                    {
                        'names': ['dependency'],
                        'required': True,
                    },
                ],
                'options': LOCAL_MANIFEST_OPTIONS,
            },
            'remove_managed_components': {
                'callback': callback,
                'hidden': True
            },
            'upload-component': {
                'callback': callback,
                'help': 'Upload component to the component registry. '
                'If the component doesn\'t exist in the registry it will be created automatically.',
                'options': SERVICE_OPTIONS + VERSION_PARAMETER + [
                    {
                        'names': ['--archive'],
                        'help': 'Path of the archive with a component to upload. '
                        'When not provided the component will be packed automatically.',
                    }, {
                        'names': ['--skip-pre-release'],
                        'help': 'Do not upload pre-release versions.',
                        'is_flag': True,
                        'default': False,
                    }, {
                        'names': ['--check-only'],
                        'help': 'Check if given component version is already uploaded and exit.',
                        'is_flag': True,
                        'default': False,
                    }, {
                        'names': ['--allow-existing'],
                        'help': 'Return success if existing version is already uploaded.',
                        'is_flag': True,
                        'default': False,
                    }
                ],
            },
            'delete-version': {
                'callback': callback,
                'help': 'Delete specified version of the component from the component registry.',
                'options': SERVICE_OPTIONS +
                [{
                    'names': ['--version'],
                    'help': 'Component version',
                    'required': True,
                }],
            },
            'upload-component-status': {
                'callback': callback,
                'help': 'Check the component uploading status by the job ID.',
                'options': SERVICE_PROFILE + [{
                    'names': ['--job'],
                    'help': 'Job ID',
                    'required': True,
                }],
            },
            'pack-component': {
                'callback': callback,
                'help': 'Create component archive and store it in the dist directory.',
                'options': SERVICE_PROFILE + NAME + VERSION_PARAMETER,
            },
        },
    }
