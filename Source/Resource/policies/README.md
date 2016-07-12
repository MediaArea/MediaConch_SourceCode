# How To

```
xsltproc policyset2policytransform2policyresult.xsl sample_policy_1.xml | sed 's/aliasxsl/xsl/g' | sed 's|xmlns:xsl="my:namespace"|xmlns:xsl="http://www.w3.org/1999/XSL/Transform"|g' | xsltproc -  some_files_mediaarea_report.xml
```
