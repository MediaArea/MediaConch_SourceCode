<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
    <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
    <xsl:template match="/mc:MediaConch/mc:policyChecks">
        <xsl:text>MediaConch report&#xa;*********************&#xa;&#xa;</xsl:text>
        <xsl:value-of select="mc:title"/>
        <xsl:text>&#xa;</xsl:text>
        <xsl:value-of select="mc:description"/>
        <xsl:text>&#xa;</xsl:text>
		<xsl:text>&#xa;****************************************&#xa;</xsl:text>
        <xsl:for-each select="mc:media">
            <xsl:value-of select="@ref"/>
            <xsl:for-each select="mc:policy">
                <xsl:value-of select="@title"/>&#xa;Context (field): <xsl:value-of select="mc:context/@field"/>&#xa;Context (value): <xsl:value-of select="mc:context/@value"/>
                    <xsl:for-each select="mc:test">
                        <xsl:text>&#xa;Track type: </xsl:text><xsl:value-of select="@tracktype"/>
                        <xsl:if test="@streamid">
                        <xsl:text>&#xa;Stream ID: </xsl:text><xsl:value-of select="@streamid"/>
                        </xsl:if>
                        <xsl:text>&#xa;Actual: </xsl:text><xsl:value-of select="@actual"/>
                        <xsl:text>&#xa;Outcome: </xsl:text>
                        <xsl:value-of select="@outcome"/>
                        <xsl:text>&#xa;Reason: </xsl:text><xsl:value-of select="@reason"/>
                    </xsl:for-each>
                <xsl:text>&#xa;****************************************&#xa;</xsl:text>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
    <xsl:template match="/mc:MediaConch/mc:implementationChecks">
        <xsl:text>MediaConch report&#xa;*********************&#xa;&#xa;</xsl:text>
        <xsl:value-of select="mc:title"/>
        <xsl:text>&#xa;</xsl:text>
        <xsl:value-of select="mc:description"/>
        <xsl:text>&#xa;</xsl:text>
		<xsl:text>&#xa;****************************************&#xa;</xsl:text>
        <xsl:for-each select="mc:media">
            <xsl:value-of select="@ref"/>
            <xsl:for-each select="mc:check">
                <xsl:value-of select="@icid"/>&#xa;Context (name): <xsl:value-of select="mc:context/@name"/>
                    <xsl:for-each select="mc:test">
                        <xsl:text>&#xa;</xsl:text>
                        <xsl:if test="mc:value/@name != ''">
							<xsl:value-of select="mc:value/@name"/>
							<xsl:text>, </xsl:text>
                        </xsl:if>
                        <xsl:text>Outcome: </xsl:text>
                        <xsl:value-of select="@outcome"/>
                        <xsl:text>&#xa;Reason: </xsl:text><xsl:value-of select="@reason"/>
                    </xsl:for-each>
                <xsl:text>&#xa;****************************************&#xa;</xsl:text>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
