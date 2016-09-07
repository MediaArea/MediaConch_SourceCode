# How To

```
xsltproc policyset2policytransform2policyresult.xsl sample_policy_1.xml | sed 's/aliasxsl/xsl/g' | sed 's|xmlns:xsl="my:namespace"|xmlns:xsl="http://www.w3.org/1999/XSL/Transform"|g' | xsltproc -  some_files_mediaarea_report.xml
```

To use comparison method, set `compare` as a test value and reference to mediaarea report to compare to:
```
xsltproc --stringparam compare mediaarea_xml_of_reference.xml policyset2policytransform2policyresult.xsl sample_policy_1.xml | sed 's/aliasxsl/xsl/g' | sed 's|xmlns:xsl="my:namespace"|xmlns:xsl="http://www.w3.org/1999/XSL/Transform"|g' | xsltproc -  some_files_mediaarea_report.xml
```


## occurrence

Optional to store.

If no `occurrence` attribute is provided, the test will run to see if the rule is true for `ANY` occurrence.

Otherwise set `occurrence` to the track number, counting from 1.

## operator

Optional to store (but storing no operator infers that you are only testing for the existance of the value).

The operator stored must be a valid xpath operator such as:

- =
- &lt;
- &gt;
- &lt;=
- &gt;=

If no operator attribute is stored, then the test only checks for the existance of the metadata value and any value to compare it to is ignored.

## scope

Optional to store.

Set to 'mt' if doing a MediaTrace test, otherwise omit the attribute.

## value

Mandatory to store.

Set to the name of the MediaInfo metadata value. Or is scope='mt' then set to a slash delimited list of the name attributes of the block and data elements, such as `Segment/Cluster/SimpleBlock/Data/keyframe` or `EBML/EBMLVersion/Data`. Note that `EBML/EBMLVersion` would not give the expected results.

For advanced use, the `size` of a MediaTrace block could be tested like `EBML/EBMLVersion/Header/Size` as long as the "block[@name]{*}/data[@name]" pattern is followed.

## tracktype

Mandatory to store. Must be set to a MediaInfo tracktype: General, Video, Audio, etc.

