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

#include "ASTDiff.h"

#include <QDebug>

#include <iostream>


// === CALL-BACK FUNCTIONS ===

int gitDiffFileParsingCB(
				 const git_diff_delta *delta,
				 float progress,
				 void *payload)
{
	std::cout << "\t" << delta->new_file.path << std::endl;
	(void) payload;
	(void) progress;
	return 0;
}

int gitDiffLineParsingCB(
				 const git_diff_delta *delta,
				 const git_diff_hunk *hunk,
				 const git_diff_line *line,
				 void *payload)
{
	ParsingData* data = (ParsingData*) payload;

	// create ASTNode and add to diffParsingData
	ASTNode* node = new ASTNode(line->content, line->content_len, line->origin);

	data->astNodes.insertMulti(node->getID(), node);

	(void) delta;
	(void) hunk;
	return 0;
}


// === ASTDiff Implementation ===

void ASTDiff::buildFromGitDiff(git_diff* gitDiff)
{
	ASTDiff::clear();

	ASTDiff::gitDiff_ = gitDiff;

	//ASTDiff::gitDiffParsingData_.astNodes = new QHash<int, ASTNode*>();

	std::cout << "Parsing Git Diff:" << std::endl;
	ASTDiff::parseGitDiff();

	std::cout << "Detecting Fundamental Operations" << std::endl;
	ASTDiff::detectFundamentalOperations();

	std::cout << "Refining Classification" << std::endl;
	ASTDiff::refineClassification();
}

void ASTDiff::print()
{
	QList<ASTNodeDiff*> nodeDiffs = ASTDiff::nodeDiffHashTable_.values();
	QList<ASTNodeDiff*>::iterator nodeDiffIter;
	for (nodeDiffIter = nodeDiffs.begin(); nodeDiffIter != nodeDiffs.end(); nodeDiffIter++)
	{
		(*nodeDiffIter)->print();
	}
}

void ASTDiff::clear()
{
	ASTDiff::gitDiff_ = nullptr;

	ASTDiff::nodeDiffHashTable_.clear();

	ASTDiff::modifiedNodes_.clear();

	ASTDiff::gitDiffParsingData_.astNodes.clear();
}

void ASTDiff::parseGitDiff()
{
	git_diff_foreach(ASTDiff::gitDiff_, gitDiffFileParsingCB, NULL, gitDiffLineParsingCB, &(ASTDiff::gitDiffParsingData_));
}

void ASTDiff::detectFundamentalOperations()
{
	QHash<int, ASTNode*> nodes = ASTDiff::gitDiffParsingData_.astNodes;

	QList<int> nodeIDs = nodes.uniqueKeys();
	QHash<int, ASTNode*>::iterator nodeIter = nodes.begin();

	ASTNode* node = nullptr;
	ASTNodeDiff* nodeDiff = nullptr;

	for(int id : nodeIDs)
	{
		switch(nodes.count(id))
		{
			case 1:
				nodeIter = nodes.find(id);
				node = nodeIter.value();
				switch(node->getDiffLineType())
				{
					case AddedLine:
						nodeDiff = new ASTNodeDiff(id);
						nodeDiff->setNodeChangeType(AddedNode);
						nodeDiff->setNewNode(node);

						ASTDiff::nodeDiffHashTable_.insert(id, nodeDiff);
						break;
					case DeletedLine:
						nodeDiff = new ASTNodeDiff(id);
						nodeDiff->setNodeChangeType(DeletedNode);
						nodeDiff->setOldNode(node);

						ASTDiff::nodeDiffHashTable_.insert(id, nodeDiff);
						break;
					default:
						Q_ASSERT(false);
				}
				break;
			case 2:
				nodeDiff = new ASTNodeDiff(id);
				nodeDiff->setNodeChangeType(ModifiedNode);

				nodeIter = nodes.find(id);
				node = nodeIter.value();

				Q_ASSERT(node->getID() == id);

				if (node->getDiffLineType() == AddedLine)
					nodeDiff->setNewNode(node);
				else
					nodeDiff->setOldNode(node);

				ASTDiff::modifiedNodes_.insertMulti(id, node);

				nodeIter++;
				node = nodeIter.value();

				if (node->getDiffLineType() == AddedLine)
					nodeDiff->setNewNode(node);
				else
					nodeDiff->setOldNode(node);
				ASTDiff::modifiedNodes_.insertMulti(id, node);
				ASTDiff::nodeDiffHashTable_.insert(id, nodeDiff);
				break;
			default:
				Q_ASSERT(false);
		}
	}
}

void ASTDiff::refineClassification()
{

	QList<int> nodeIDs = ASTDiff::modifiedNodes_.uniqueKeys();
	QHash<int, ASTNode*>::iterator nodeIter = ASTDiff::modifiedNodes_.begin();
	QHash<int, ASTNodeDiff*>::iterator nodeDiffIter = ASTDiff::nodeDiffHashTable_.begin();

	ASTNode* nodeA = nullptr;
	ASTNode* nodeB = nullptr;

	ASTNodeDiff* nodeDiff;
	for (int id : nodeIDs)
	{
		nodeIter = ASTDiff::modifiedNodes_.find(id);
		nodeA = nodeIter.value();
		nodeIter++;
		nodeB = nodeIter.value();

		nodeDiffIter = ASTDiff::nodeDiffHashTable_.find(id);
		nodeDiff = nodeDiffIter.value();

		// check for same name -> reordering detection
		int reorder = QString::compare(nodeA->getName(), nodeB->getName());
		if (reorder != 0)
			nodeDiff->setReordered(true);
		else
			nodeDiff->setReordered(false);

		// check for same type -> type change
		int typeChange = QString::compare(nodeA->getType(), nodeB->getType());
		if (typeChange != 0)
			nodeDiff->setTypeChanged(true);
		else
			nodeDiff->setTypeChanged(false);

		// check for same value -> update
		int valueUpdate = QString::compare(nodeA->getValue(), nodeB->getValue());
		if (valueUpdate != 0)
			nodeDiff->setValueUpdated(true);
		else
			nodeDiff->setValueUpdated(false);
	}

	// move detection
	ASTDiff::moveDetection();
}

void ASTDiff::moveDetection()
{
	// TODO
}
