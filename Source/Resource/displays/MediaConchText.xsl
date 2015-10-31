<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
    <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
    <xsl:template match="/mc:MediaConch/mc:policyChecks">
        <xsl:text>*********************&#xa;</xsl:text>
        <xsl:text>* MediaConch report *&#xa;</xsl:text>
        <xsl:text>*********************&#xa;</xsl:text>
		<xsl:if test="mc:title != '' or mc:description != ''">
			<xsl:text>&#xa;</xsl:text>
			<xsl:if test="mc:title != ''">
				<xsl:value-of select="mc:title"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:if>
			<xsl:if test="mc:description != ''">
				<xsl:value-of select="mc:description"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:if>
			<xsl:text>&#xa;</xsl:text>
		</xsl:if>
        <xsl:for-each select="mc:media">
			<xsl:text>******************************************************************************&#xa;</xsl:text>
            <xsl:value-of select="@ref"/>
			<xsl:text>&#xa;</xsl:text>
            <xsl:for-each select="mc:check">
				<xsl:text>------------------------------------------------------------------------------&#xa;</xsl:text>
                <xsl:value-of select="@title"/>
				<xsl:text>&#xa;</xsl:text>
				<xsl:if test="mc:context/@field != ''">
					<xsl:text>Context (field): </xsl:text>
					<xsl:value-of select="mc:context/@field"/>
					<xsl:text>&#xa;</xsl:text>
				</xsl:if>
				<xsl:if test="mc:context/@value != ''">
					<xsl:text>Context (value): </xsl:text>
					<xsl:value-of select="mc:context/@value"/>
					<xsl:text>&#xa;</xsl:text>
				</xsl:if>
				<xsl:for-each select="mc:test">
					<xsl:if test="@tracktype != ''">
						<xsl:text>Track type: </xsl:text>
						<xsl:value-of select="@tracktype"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:if>
					<xsl:if test="@streamid">
						<xsl:text>Stream ID: </xsl:text>
						<xsl:value-of select="@streamid"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:if>
					<xsl:if test="@actual != ''">
						<xsl:text>Actual: </xsl:text>
						<xsl:value-of select="@actual"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:if>
					<xsl:text>Outcome: </xsl:text>
					<xsl:value-of select="@outcome"/>
					<xsl:text>&#xa;</xsl:text>
					<xsl:if test="@reason != ''">
						<xsl:text>Reason: </xsl:text>
						<xsl:value-of select="@reason"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:if>
				</xsl:for-each>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
    <xsl:template match="/mc:MediaConch/mc:implementationChecks">
        <xsl:text>*********************&#xa;</xsl:text>
        <xsl:text>* MediaConch report *&#xa;</xsl:text>
        <xsl:text>*********************&#xa;</xsl:text>
		<xsl:if test="mc:title != '' or mc:description != ''">
			<xsl:text>&#xa;</xsl:text>
			<xsl:if test="mc:title != ''">
				<xsl:value-of select="mc:title"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:if>
			<xsl:if test="mc:description != ''">
				<xsl:value-of select="mc:description"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:if>
			<xsl:text>&#xa;</xsl:text>
		</xsl:if>
        <xsl:for-each select="mc:media">
			<xsl:text>******************************************************************************&#xa;</xsl:text>
            <xsl:value-of select="@ref"/>
			<xsl:text>&#xa;</xsl:text>
            <xsl:for-each select="mc:check">
				<xsl:text>------------------------------------------------------------------------------&#xa;</xsl:text>
                <xsl:value-of select="@icid"/><xsl:text>&#xa;</xsl:text>
				<xsl:if test="mc:context/@name != ''">
					<xsl:text>Context (name): </xsl:text>
					<xsl:value-of select="mc:context/@name"/>
					<xsl:text>&#xa;</xsl:text>
				</xsl:if>
				<xsl:for-each select="mc:test">
					<xsl:for-each select="mc:value">
						<xsl:if test="@name != ''">
							<xsl:value-of select="@name"/>
							<xsl:text>=</xsl:text>
						</xsl:if>
						<xsl:value-of select="."/>
						<xsl:text>, </xsl:text>
					</xsl:for-each>
					<xsl:text>Outcome: </xsl:text>
					<xsl:value-of select="@outcome"/>
					<xsl:text>&#xa;</xsl:text>
					<xsl:if test="@reason != ''">
						<xsl:text>Reason: </xsl:text>
						<xsl:value-of select="@reason"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:if>
				</xsl:for-each>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
