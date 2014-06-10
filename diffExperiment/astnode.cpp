/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "astnode.h"

#include <QDebug>
#include <QString>

ASTNode::ASTNode()
{
	ASTNode::depth_ = -1;
	ASTNode::name_ = QString();
	ASTNode::type_ = QString();
	ASTNode::id_ = -1;
	ASTNode::value_ = QString();

	ASTNode::diffLineType_ = UnchangedLine;
}

ASTNode::ASTNode(const char* line, size_t lineLength, char diffLineType)
{
	ASTNode::parseEnvisionLine(line, lineLength);

	switch (diffLineType) {
		case '+': ASTNode::diffLineType_ = AddedLine;
					 break;
		case '-': ASTNode::diffLineType_ = DeletedLine;
					 break;
		case ' ': ASTNode::diffLineType_ = UnchangedLine;
					 break;
	}
}

ASTNode::~ASTNode()
{
	//TODO
}


void ASTNode::printNode()
{
	qDebug() << "Node ID:" << ASTNode::id_ << ASTNode::name_ << ASTNode::type_ << ASTNode::value_;
}



void ASTNode::parseEnvisionLine(const char *line, size_t lineLength)
{
	// read tabs
	size_t i = 0;
	while (i < lineLength && (line[i] == '\t' || line[i] == ' ')) i++;
	ASTNode::depth_ = i;

	// read name
	size_t offset = i;
	while (i < lineLength && (line[i] != '\t' && line[i] != ' ')) i++;
	ASTNode::name_ = QString::fromLatin1(line + offset, i-offset);

	// skip space
	while (i < lineLength && (line[i] == '\t' || line[i] == ' ')) i++;

	// read type
	offset = i;
	while (i < lineLength && (line[i] != '\t' && line[i] != ' ')) i++;
	ASTNode::type_ = QString::fromLatin1(line + offset, i-offset);

	// skip space
	while (i < lineLength && (line[i] == '\t' || line[i] == ' ')) i++;

	// read id
	offset = i;
	while (i < lineLength && (line[i] != '\t' && line[i] != ' ' && line[i] != '.')) i++;
	ASTNode::id_ = QString::fromLatin1(line + offset, i-offset).toInt();

	if (line[i] == '.')
	{
		// skip dot
		i++;

		// skip space
		while (i < lineLength && (line[i] == '\t' || line[i] == ' ')) i++;

		// read value
		offset = i;
		while (i < lineLength && (line[i] != '\t' && line[i] != ' ' && line[i] != '.' && line[i] != '\n')) i++;
		ASTNode::value_ = QString::fromLatin1(line + offset, i-offset);
	}
}
