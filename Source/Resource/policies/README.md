# How To

```
xsltproc policyset2policytransform2policyresult.xsl sample_policy_1.xml | sed 's/aliasxsl/xsl/g' | sed 's|xmlns:xsl="my:namespace"|xmlns:xsl="http://www.w3.org/1999/XSL/Transform"|g' | xsltproc -  some_files_mediaarea_report.xml
```

To use comparison method, set `compare` as a test value and reference to mediaarea report to compare to:
```
xsltproc --stringparam compare mediaarea_xml_of_reference.xml policyset2policytransform2policyresult.xsl sample_policy_1.xml | sed 's/aliasxsl/xsl/g' | sed 's|xmlns:xsl="my:namespace"|xmlns:xsl="http://www.w3.org/1999/XSL/Transform"|g' | xsltproc -  some_files_mediaarea_report.xml
```
