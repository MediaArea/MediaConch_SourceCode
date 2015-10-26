<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
    <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
    <xsl:template match="/mc:MediaConch/mc:policyChecks">
Policy : <xsl:value-of select="mc:title"/>
Description: <xsl:value-of select="mc:description"/>
        <xsl:for-each select="mc:media">
- Ref: <xsl:value-of select="@ref"/>
            <xsl:for-each select="mc:policy">
- Title: <xsl:value-of select="@title"/>
Context (field): <xsl:value-of select="mc:context/@field"/>
Context (value): <xsl:value-of select="mc:context/@value"/>
</xsl:for-each>
<xsl:for-each select="mc:test">
tracktype: <xsl:value-of select="@tracktype"/>
streamid: <xsl:value-of select="@streamid"/>
actual: <xsl:value-of select="@actual"/>
outcome: <xsl:value-of select="@outcome"/>
reason: <xsl:value-of select="@reason"/>
</xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
