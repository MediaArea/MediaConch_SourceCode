## REST API

### Usage

This API is used by the daemon with the others client to communicate.

It is used above an HTTP connection.

### History

#### Version 1.14
 * Create new command for the checker
  * Checker_Stop

#### Version 1.13
 * Update command:
  * Mediaconch_Watch_Folder: add options in request
  * Checker_Id_From_Filename: add options in request
  * Checker_File_Information: add options in result
  * Use a common error MediaConch_Nok everywhere

#### Version 1.12
 * Update command:
  * Checker_Analyze: add a flag to not analyze with MIL

#### Version 1.11
 * Update command:
  * Policy_Get: add is_public
  * Policy_Get: add must_be_public
  * Policy_Dump: add must_be_public
  * Policy_Duplicate: add must_be_public
  * Policy_Duplicate: add dst_user
  * Policy_Change_Info: add license
  * Policy_Policy: add license
  * Checker_Analyze: add array of options for analyze
 * Create new command for the Policy
  * Policy_Get_Public_Policies
  * Policy_Change_Is_Public
 * create the policy object:
  * Policy_Public_Policy

#### Version 1.10
 * Update command:
  * Checker_Get_Report: add plugins to the command to use
 * Create new command for the MediaConch
  * MediaConch_Get_Plugins
  * MediaConch_Watch_Folder
  * MediaConch_List_Watch_Folders
  * MediaConch_Edit_Watch_Folder
  * MediaConch_Remove_Watch_Folder
 * Create new structure for the MediaConch
  * MediaConch_Nok
 * Create new command namespace:
  * MediaConch

#### Version 1.9
 * Create new command for the checker
  * Checker_Id_From_Filename
  * Checker_File_information
 * Update commands of the checker
  * Checker_Status_result: add more information
 * Add user to all checker_* command

#### Version 1.8

* Add XSL's options in Checker_Get_Report and Checker_Validate
* Put the verbosity to options

#### Version 1.7

* create the command for policies
  * XSLT_Policy_Create
  * Policy_Import
  * Policy_Save
  * Policy_Remove
  * Policy_Duplicate
  * Policy_Move
  * Policy_Dump
  * Policy_Get_Name
  * Policy_Change_Info
  * Policy_Change_Type
  * Policy_Get
  * Policy_Get_Policies
  * Policy_Get_Policies_Names_List
  * Policy_Get_Policies_Count
  * Policy_Clear_Policies
  * XSLT_Policy_Rule_Create
  * XSLT_Policy_Rule_Get
  * XSLT_Policy_Rule_Edit
  * XSLT_Policy_Rule_Move
  * XSLT_Policy_Rule_Delete

* create the policy object:
  * Policy_Policy
  * XSLT_Policy_Rule
  * Policy_Error

#### Version 1.6

* Use id for policy instead of files
* Make scope (checker) for all checker commands
* Change the name of create policy from file

#### Version 1.5

* Add the create policy from file command

#### Version 1.4

* Add the default values for type command

#### Version 1.3

* Add verbosity in report request
* Add tool in status ok
* Add VERAPDF and DPFMANAGER report kinds

#### Version 1.2

* Add the List command
* Add the Validate command
* Add the File_From_Id command

#### Version 1.1

* Add the force parameter in the Analyze request.

#### Version 1

* API created.
* Command: Analyze, Status, Report, Retry, Clear

### API

Current API version: $API_VERSION = 1.14

#### Command

* MediaConch_Watch_Folder:        HTTP POST
* MediaConch_List_Watch_Folder:   HTTP GET
* MediaConch_Edit_Watch_Folder:   HTTP POST
* MediaConch_Remove_Watch_Folder: HTTP POST

* Checker_Analyze:                HTTP POST
* Checker_Status:                 HTTP GET
* Checker_Report:                 HTTP POST
* Checker_Retry:                  HTTP PUT
* Checker_Clear:                  HTTP DELETE
* Checker_List:                   HTTP GET
* Checker_Validate:               HTTP POST
* Checker_File_From_Id:           HTTP POST
* Default_Values_For_type:        HTTP GET
* Checker_Stop:                   HTTP POST

* XSLT_Policy_Create:             HTTP GET
* Policy_Import:                  HTTP POST
* Policy_Remove:                  HTTP GET
* Policy_Duplicate:               HTTP GET
* Policy_Move:                    HTTP GET
* Policy_Dump:                    HTTP GET
* Policy_Save:                    HTTP GET
* Policy_Get_Name:                HTTP GET
* Policy_Change_Info:             HTTP POST
* Policy_Change_Type:             HTTP POST
* Policy_Change_Is_Public:        HTTP POST
* Policy_Get:                     HTTP GET
* Policy_Get_Policies:            HTTP GET
* Policy_Get_Public_Policies:     HTTP GET
* Policy_Get_Policies_Names_List: HTTP GET
* Policy_Get_Policies_Count:      HTTP GET
* Policy_Clear_Policies:          HTTP GET
* Policy_Create_From_File:        HTTP GET

#### MediaConch_Nok

Structure used when error occurs.

##### Parameters

* error:              String with the error description
* id:                 Integer, optionnal with an id (file, policy, ...)

Example:
{"error": "File ID not existing", id: 2}

#### MediaConch_Get_Plugins

JSON format for the parameters.
URL: /$API_VERSION/mediaconch_get_plugins

##### Request

Parameters:

No parameters for the request

##### Response

Parameters:

* plugins:           Array of String with the Plugins ID
* nok:               MediaConch_Nok when error occurs

#### MediaConch_Watch_Folder

JSON format for the parameters.
URL: /$API_VERSION/mediaconch_watch_folder

##### Request

Parameters:

* folder:            String with the new directory name to watch
* folder_reports:    String with the name of the directory where to put the reports of the file analyzed in the watched folders
* plugins:           Array of String with the plugins ID
* policies:          Array of String with the policies contents
* user:              Integer: Use this User ID for the watch folder. If not present, find a unique ID
* recursive:         Boolean: Check the folder recursively (sub-directory), set to true by default
- options:           Array of Object of 2 Strings: List of Options to be given to MediaInfoLib

##### Response

Parameters:

* user:              Integer: a unique id of the user for this watch folder or the one given as input
* nok:               MediaConch_Nok when error occurs

#### MediaConch_List_Watch_Folders

URI format for the parameters.
URL: /$API_VERSION/mediaconch_list_watch_folders

##### Request

Parameters:

No parameters for the request

##### Response

Parameters:

* folders:           Array of String: List the folders watched
* nok:               MediaConch_Nok when error occurs

#### MediaConch_Edit_Watch_Folder

JSON format for the parameters.
URL: /$API_VERSION/mediaconch_edit_watch_folder

##### Request

Parameters:

* folder:            String with the old directory watched
* folder_reports:    String with the name of the directory where to put the reports of the file analyzed in the watched folders

##### Response

Parameters:

* nok:               MediaConch_Nok when error occurs

#### MediaConch_Remove_Watch_Folder

JSON format for the parameters.
URL: /$API_VERSION/mediaconch_remove_watch_folder

##### Request

Parameters:

* folder:            String with the directory to stop to watch

##### Response

Parameters:

* nok:               MediaConch_Nok when error occurs

#### Checker_Analyze

JSON format for the parameters.
URL: /$API_VERSION/checker_analyze

##### Request

Parameters:

* args:              Array of arguments

- file:              String: Name of the file to analyze
- user:              Integer: a unique id for the user
- id:                Integer: a unique id for the request
- plugins:           Array of Integer: List of Plugins to use (other than format plugins)
- options:           Array of Object of 2 Strings: List of Options to be given to MediaInfoLib
- force:             Boolean: force to analyze the file even if registered in database (introduced in v1.1)
- mil_analyze:       Boolean: force to not analyze with MediaInfoLib

##### Response

Parameters:

* ok:                Array of valid arguments

- inId:              Integer: id given by the request
- outId:             Integer: unique id generated by the daemon
- create:            Boolean: if the file was not registered

* nok:               Array of MediaConch_Nok when error occurs with ids

#### Checker_Status

URI format for the parameters.
URL: /$API_VERSION/checker_status

##### Request

Parameters:

- user:              Integer: a unique id for the user

* List of:

- id:                Integer: id given by the Analyze command

##### Response

Parameters:

* ok:                Array of valid arguments:

- id:                Integer: id given by the request
- finished:          Boolean: if the file is finished to be analyzed
- has_error:         Boolean: if the file analyze has error (optional, no error if not present)
- error_log:         String: if the file has error (has_error = true), give the error logs if any
- done:              Double: Percent done by the analysis
- tool:              REPORT: give the report tool used to analyze, when it is finished, optionnal (Default is IMPLEMENTATION) (since API v1.3)
- generated_id:      Array of Integers: ids of files generated, if any.
- source_id:         Integer: id of the source file if generated by the daemon

* nok:               Array of MediaConch_Nok when error occurs with ids

#### Checker_Report

JSON format for the parameters.
URL: /$API_VERSION/checker_report

##### Request

Parameters:

- user:              Integer: a unique id for the user

* args:              Array of arguments

- id:                Integer: id given by the Analyze command
- reports:           Array of report kinds
- policies_contents: Array of policies contents (Strings)
- policies_ids:      Array of policies ids (Integer)
- display_content:   String:  display content
- display_name:      String:  display name (HTML, TEXT, XML, MAXML, JSTREE, SIMPLE, CSV)
- options:           Object of strings given to the XSL transformations: Verbosity should be defined here.
                     Example: "options":{"verbosity": "5", "key":"value", ...}

##### Response

Parameters:

* ok:                Structure of the following parameters

- report:            String: Contain the report corresponding to all requested parameters
- valid:             Boolean: true if all policies are valid (optionnal, depending on reports kind)

* nok:               MediaConch_Nok when error occurs

#### Checker_Retry

JSON format for the parameters.
URL: /$API_VERSION/checker_retry

##### Request

Parameters:

- user:              Integer: a unique id for the user
- ids:               Array of id given by the Analyze command

##### Response

Parameters:

* ok:                Array of Integer: ids of the files analyzed again

* nok:               MediaConch_Nok when error occurs

#### Checker_Clear

URI format for the parameters.
URL: /$API_VERSION/checker_clear

##### Request

Parameters:

* List of:

- user:              Integer: a unique id for the user
- ids:               Array of integers: ids given by the Analyze command

##### Response

Parameters:

* ok:                Array of Integer: ids of the files removed from Database

* nok:               MediaConch_Nok when error occurs

#### Checker_Stop

URI format for the parameters.
URL: /$API_VERSION/checker_stop

##### Request

Parameters:

- user:              Integer: a unique id for the user

* List of ids:       Array of integers: ids given by the Analyze command

##### Response

Parameters:

* ok:                Array of Integer: ids of the files stoped

* nok:               MediaConch_Nok when error occurs

#### Checker_List

No Parameter
URL: /$API_VERSION/checker_list

##### Request

Parameters:

- user:              Integer: a unique id for the user

##### Response

Parameters:

* files:             Array of files registered

- file:              Filename of the file
- id:                Internal id, id to be used by other request

* nok:               MediaConch_Nok when error occurs

#### Checker_Validate

JSON format for the parameters.
URL: /$API_VERSION/checker_validate

##### Request

Parameters:

- user:              Integer: a unique id for the user
* args:              Array of arguments

- id:                Integer: id given by the Analyze command
- report:            report kind, only POLICY or IMPLEMENTATION are possible
- policies_contents: Array of policies contents (Strings)
- policies_names:    Array of policies names (Strings)
- options:           Objects with strings (key, value) given to the XSL transformations: Verbosity should be defined here.
                     Example: "options":{"verbosity": "5", "key":"value", ...}

##### Response

Parameters:

* ok:                Structure of the following parameters

- id:                Integer: Id given by the request
- valid:             Boolean: true if all policies are valid for this file

* nok:               MediaConch_Nok when error occurs

#### Checker_File_From_Id

JSON format for the parameters.
URL: /$API_VERSION/checker_file_from_id

##### Request

Parameters:

- user:              Integer: a unique id for the user
- id:                Integer: id given by the Analyze command

##### Response

Parameter:

* file:              String: File corresponding to the id, left empty if id not matching
* nok:               MediaConch_Nok when error occurs

#### Checker_Id_From_Filename

JSON format for the parameters.
URL: /$API_VERSION/checker_id_from_filename

##### Request

Parameters:

- user:              Integer: a unique id for the user
- filename:          String: Requested filename for id
- options:           Array of String: Options associated to the analyze

##### Response

Parameter:

* id:                Integer: id of the file, same value as the one from analyze command
* nok:               MediaConch_Nok when error occurs

#### Checker_File_Information

JSON format for the parameters.
URL: /$API_VERSION/checker_file_information

##### Request

Parameters:

- user:              Integer: a unique id for the user
- id:                Integer: id given by the Analyze command
- options:           Array of String: Options associated to the analyze

##### Response

Parameter:

* filename:               String: File corresponding to the id
* file_last_modifciation: String: Time of the file last modification, correspond to the creation time for generated file
* generated_id:           Integer: id of the last file generated, if any
* source_id:              Integer: id of the source file if generated by the daemon
* generated_time:         Integer: Time in milliseconds to generate this file
* generated_log:          String: Output log when generating this file (stdout)
* generated_error_log:    String: Output error log when generating this file (stderr)
* analyzed:               Boolean: If this file is already analyzed
* has_error:              Boolean: if the file analyze has error (optional, no error if not present)
* error_log:              String: if the file has error (has_error = true), give the error logs if any

* nok:                   MediaConch_Nok when error occurs

#### Default_Values_For_type

URI format for the parameters.
URL: /$API_VERSION/default_values_for_type

##### Request

Parameters:

* type:              String: name of the type wanted
* field:             String: name of the field wanted

##### Response

Parameters:

* values:            Array of values (String)
* nok:               MediaConch_Nok when error occurs

#### XSLT_Policy_Create

##### Request

Parameters:

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_create

##### Request

Parameters:

user:      User ID
parent_id: Policy ID of the parent
type:      Policy operator ("and", "or"; default is "and")

##### Response

Parameters:

- if command is ok, return an object with the id of the created policy: '{"XSLT_POLICY_CREATE_RESULT": {"id": 0}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_CREATE_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Import

JSON format for the parameters.
URL: /$API_VERSION/policy_import

##### Request

Parameters:

user:  User ID
xml:   XML corresponding to the policy
{\"POLICY_IMPORT\": {\"xml\": \"$XML_DATA\"}}

##### Response

Parameters:

- if command is ok, return an object with the id of the created policy: '{"POLICY_IMPORT_RESULT": {"id": 0}}'
- otherwise, return a MediaConch_Nok
{"POLICY_IMPORT_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Remove

URI format for the parameters.
URL: /$API_VERSION/policy_remove?id=0

##### Request

Parameters:

user:  User ID
id:    Policy ID to remove

##### Response

Parameters:

- If command is ok, return an Empty object '{"POLICY_REMOVE_RESULT": {}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_REMOVE_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Dump

URI format for the parameters.
URL: /$API_VERSION/policy_dump?id=0&must_be_public=true

##### Request

Parameters:

user:           User ID
id:             Policy ID to dump
must_be_public: Return an error if the policy is not public

##### Response

Parameters:

- If command is ok, return an object with the XML of the policy '{"POLICY_DUMP_RESULT": {"xml": "POLICY_XML"}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_DUMP_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Save

URI format for the parameters.
URL: /$API_VERSION/policy_save?id=0

##### Request

Parameters:

user:  User ID
id:    Policy ID to save

##### Response

Parameters:

- If command is ok, return an Empty object '{"POLICY_SAVE_RESULT": {}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_SAVE_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Duplicate

URI format for the parameters.
URL: /$API_VERSION/policy_duplicate?id=0

##### Request

Parameters:

user:           User ID
id:             Policy ID to duplicate
dst_policy_id:  Policy ID used as parent (default is -1, root node)
dst_user:       Copy the policy to this user ID (optional, if not present, copy to the "user")
must_be_public: Return an error if the policy is not public

##### Response

Parameters:

- If command is ok, return an object with the new ID '{"POLICY_DUPLICATE_RESULT": {"id": 0}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_DUPLICATE_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Move

URI format for the parameters.
URL: /$API_VERSION/policy_move?id=0&dst_policy_id=41

##### Request

Parameters:

user:          User ID
id:            Policy ID to move (duplicate + delete)
dst_policy_id: Policy ID used as new parent (default is -1, root node)

##### Response

Parameters:

- If command is ok, return an object with the new ID '{"POLICY_MOVE_RESULT": {"id": 42}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_MOVE_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Get_Name

URI format for the parameters.
URL: /$API_VERSION/policy_get_name?id=0

##### Request

Parameters:

user:  User ID
id:    Policy ID to retrieve information

##### Response

Parameters:

- If command is ok, return an object with the name '{"POLICY_GET_NAME_RESULT": {"name": "New policy"}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_GET_NAME_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Change_Info

JSON format for the parameters.
URL: /$API_VERSION/policy_change_info"

##### Request

Parameters:

user:        User ID
id:          Policy ID to retrieve information
name:        New name for the policy
description: New description for the policy
license:     New license for the policy
'{"POLICY_CHANGE_INFO":{"id": 0, "name": "changed name", "description": "changed description", "license": "MIT"}}'

##### Response

Parameters:

- If command is ok, return an empty object '{"POLICY_CHANGE_INFO_RESULT": {}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_CHANGE_INFO_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Change_Type

JSON format for the parameters.
URL: /$API_VERSION/policy_change_type"

##### Request

Parameters:

user:        User ID
id:          Policy ID to retrieve information
type:        New type for the policy
'{"POLICY_CHANGE_TYPE":{"id": 0, "type": "or"}}'

##### Response

Parameters:

- If command is ok, return an empty object '{"POLICY_CHANGE_TYPE_RESULT": {}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_CHANGE_TYPE_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Change_Is_Public

JSON format for the parameters.
URL: /$API_VERSION/policy_change_is_public"

##### Request

Parameters:

user:        User ID
id:          Policy ID to retrieve information (must be a root policy)
is_public:   Boolean to set the policy to public or not
'{"POLICY_CHANGE_IS_PUBLIC":{"id": 0, "is_public": true}}'

##### Response

Parameters:

- If command is ok, return an empty object '{"POLICY_CHANGE_IS_PUBLIC_RESULT": {}}'
- Otherwise, return a MediaConch_Nok
{"POLICY_CHANGE_IS_PUBLIC_RESULT": {"nok": {\"error\":\"ERROR\"}}}

#### Policy_Get

URI format for the parameters.
URL: /$API_VERSION/policy_get?id=0&format=JSTREE

##### Request

Parameters:

user:           User ID
id:             Policy ID
format:         Output format, can be "JSTREE" or "JSON"
must_be_public: Return an error if the policy is not public

##### Response

Parameters:

- if command is ok, if format is JSON, return a Policy_Policy: '{"POLICY_GET_RESULT": {"policy": {"type": "and", "id": 0, "description": "", "parent_id": -1, "name": "New policy", "is_system": false, "children":[]}}}'
- if command is ok, if format is JSTREE, return a string with policy tree: '{"POLICY_GET_RESULT": "{\"policyTree\": {\"type\": \"p\", \"text\": \"New policy\", \"data\": {\"kind\":\"XSLT\",\"policyId\": 0, \"description\": \"\", \"parent_id\": -1, \"isEditable\": true,\"type\":\"or\"},\"children\":[]}}"}'
- otherwise, return a MediaConch_Nok
{"POLICY_GET_POLICIES_COUNT_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Get_Policies_Count

URI format for the parameters.
URL: /$API_VERSION/policy_get_policies_count

##### Request

Parameters:

user:  User ID

##### Response

Parameters:

- if command is ok, return an object with the number of policies for the user in size: '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}'
- otherwise, return a MediaConch_Nok
{"POLICY_GET_POLICIES_COUNT_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Get_Policies

URI format for the parameters.
URL: /$API_VERSION/policy_get_policies?id=0&id=1

##### Request

Parameters:

user:   User ID
id:     Policy ID (can be cumulated), if not present, return all policies
format: Output format, can be "JSTREE" or "JSON"

##### Response

Parameters:

- if command is ok, format is "JSON", return an object with an array of policies (Policy_Policy): '{"POLICY_GET_POLICIES_RESULT": {"policies": []}}'
- if command is ok, format is "JSTREE", return an object with a jstree string policies: '{"POLICY_GET_POLICIES_RESULT": {"policiesTree": "[]"}}'
- otherwise, return a MediaConch_Nok
{"POLICY_GET_POLICIES_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Get_Public_Policies

URI format for the parameters.
URL: /$API_VERSION/policy_get_public_policies

##### Request

Parameters:

- No parameters for Policy_Get_Public_Policies

##### Response

Parameters:

- if command is ok, return an object with an array of public policies (Policy_Public_Policy): '{"POLICY_GET_PUBLIC_POLICIES_RESULT": {"policies": []}}'
- otherwise, return a MediaConch_Nok
{"POLICY_GET_POLICIES_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Get_Policies_Names_List

URI format for the parameters.
URL: /$API_VERSION/policy_get_policies_names_list

##### Request

Parameters:

user:  User ID

##### Response

Parameters:

- if command is ok, return an object with an array of policies ids and names: '{"POLICY_GET_POLICIES_NAMES_LIST_RESULT": {"policies": [{\"id\":0,\"name\":\"policy name\"}]}}'
- otherwise, return a MediaConch_Nok
{"POLICY_GET_POLICIES_NAMES_LIST_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Clear_Policies

URI format for the parameters.
URL: /$API_VERSION/policy_clear_policies

##### Request

Parameters:

user:  User ID

##### Response

Parameters:

- if command is ok, return an empty object: '{"POLICY_CLEAR_POLICIES_RESULT": {}}'
- otherwise, return a MediaConch_Nok
{"POLICY_CLEAR_POLICIES_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Create_From_File

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_create_from_file

##### Request

Parameters:

* user:              Integer: User ID
* id:                Integer: Id given by analyze for a file

##### Response

Parameters:

* policy_id:         Integer with the ID of the created XSL policy

* nok:               MediaConch_Nok when error occurs

#### XSLT_Policy_Rule_Create

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_create?policy_id=0

##### Request

Parameters:

user:      User ID
policy_id: Policy ID of the parent (XSLT) policy

##### Response

Parameters:

- if command is ok, return an object with the id of the created rule: '{"XSLT_POLICY_RULE_CREATE_RESULT": {"id": 0}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_CREATE_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Rule_Get

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_get?policy_id=0&id=0

##### Request

Parameters:

user:      User ID
policy_id: Policy ID of the parent (XSLT) policy
id:        Rule ID of the rule wanted

##### Response

Parameters:

- if command is ok, return a XSLT_Policy_Rule: '{"XSLT_POLICY_RULE_GET_RESULT": {"rule": {"field": "", "id": 0, "occurrence": -1, "name": "New Rule", "ope": "", "tracktype": "", "value": ""}}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_CREATE_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Rule_Edit

JSON format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_edit

##### Request

Parameters:

user:      User ID
policy_id: Policy ID of the parent (XSLT) policy
rule:      Contains a XSLT_Policy_Rule object, use the "id" value to find the correct rule

'{"XSLT_POLICY_RULE_EDIT": {"rule": {"field": "Format", "id": 0, "occurrence": -1, "name": "General Format is Matroska", "ope": "is_equal", "tracktype": "General", "value": "Matroska"}}}'

##### Response

Parameters:

- if command is ok, return an empty object: '{"XSLT_POLICY_RULE_EDIT_RESULT": {}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_EDIT_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Rule_Duplicate

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_duplicate?policy_id=0&id=0

##### Request

Parameters:

user:          User ID
policy_id:     Policy ID of the parent (XSLT) policy
id:            Rule ID of the rule to duplicate
dst_policy_id: Policy ID used as parent (default is -1, root node)

##### Response

Parameters:

- if command is ok, return an object with the new Rule ID: '{"XSLT_POLICY_RULE_DUPLICATE_RESULT": {"id": 17}}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_DUPLICATE_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Rule_Move

##### Request

Parameters:

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_move?policy_id=0&id=0&dst_policy_id=42

user:          User ID
policy_id:     Policy ID of the parent (XSLT) policy
id:            Rule ID of the rule to move
dst_policy_id: Policy ID used as new parent (default is -1, root node)

##### Response

Parameters:

- if command is ok, return an object with the new Rule ID: '{"XSLT_POLICY_RULE_MOVE_RESULT": {"id": 17}}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_MOVE_RESULT": {"nok": {"error":"ERROR"}}}

#### XSLT_Policy_Rule_Delete

URI format for the parameters.
URL: /$API_VERSION/xslt_policy_rule_delete?policy_id=0&id=0

##### Request

Parameters:

user:      User ID
policy_id: Policy ID of the parent (XSLT) policy
id:        Rule ID of the rule to delete

##### Response

Parameters:

- if command is ok, return an empty object: '{"XSLT_POLICY_RULE_DELETE_RESULT": {}}'
- otherwise, return a MediaConch_Nok
{"XSLT_POLICY_RULE_DELETE_RESULT": {"nok": {"error":"ERROR"}}}

#### Policy_Policy

Policy Object

Parameters:

* id:          ID of the policy
* parent_id:   ID of the parent policy (optional, root is -1)
* name:        string containing the name of the policy (optional)
* kind:        string containing the policy kind ("UNKNOWN" or "XSLT")
* type:        string containing the policy operator (optional; value: "and", "or"; default is "and")
* is_system:   boolean set to true if it is a system policy
* is_public:   boolean set to true if it is a public policy
* description: string containing the description of the policy (optional)
* license:     string containing the license of the policy (optional)
* children:    list of children of the policy (sub-policies, rules)

{"id":0, "parent_id":-1, "name": "NAME", "description":"", "type":"and", "children":[]}

#### XSLT_Policy_Rule

Policy Object

Parameters:

* id:         ID of the rule inside the policy
* name:       Name of the rule
* tracktype:  Track type of the rule
* field:      Field of the rule
* scope:      Scope of the rule, mt for MediaTrace, nothing for MediaInfo
* occurrence: Number of occurrence of the field
* ope:        Operator of the Rule
* value:      Value of the rule

{"id":0, "name": "NAME", "tracktype": "TYPE", "field":"FIELD", "occurrence":-1, "ope":"is_existing", value=""}

#### Policy_Public_Policy

Public Policy Object

Parameters:

* id:          ID of the policy
* user:        ID of the user owning this policy
* name:        string containing the name of the policy (optional)
* description: string containing the name of the policy (optional)

{"id":0, "user":-1, "name": "NAME", "description":""}

#### Policy_Error

Policy Object

Parameters:

* error: string corresponding to the error
{\"error\"=\"ERROR\"}

#### Report kinds

* NO\_REPORT:      0
* POLICY:         1
* IMPLEMENTATION: 2
* MEDIAINFO:      3
* MEDIATRACE:     4
* VERAPDF:        5
* DPFMANAGER:     6

#### Error reason

* NO\_REASON           0
* FILE\_NOT\_EXISTING   1
* ID\_NOT\_EXISTING     2
* NOT\_READY            3
