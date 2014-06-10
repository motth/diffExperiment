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

#include "astdiff.h"

#include <QDebug>


// === CALL-BACK FUNCTIONS ===

int gitDiffFileParsingCB(
				 const git_diff_delta *delta,
				 float progress,
				 void *payload)
{
	qDebug() << delta->new_file.path;
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
	diffParsingData* data = (diffParsingData*) payload;

	// create ASTNode and add to diffParsingData
	ASTNode* node = new ASTNode(line->content, line->content_len, line->origin);

	data->astNodes->insertMulti(node->getID(), node);

	(void) delta;
	(void) hunk;
	return 0;
}


// === ASTDiff Implementation ===

ASTDiff::ASTDiff()
{	
	ASTDiff::gitDiff_ = nullptr;

	ASTDiff::nodeDiffHashTable_ = new QHash<int, ASTNodeDiff*>();

	ASTDiff::modifiedNodes_ = new QHash<int, ASTNode*>();
}

ASTDiff::~ASTDiff()
{
	// TODO
}

void ASTDiff::buildFromGitDiff(git_diff* gitDiff)
{
	ASTDiff::clear();

	ASTDiff::gitDiff_ = gitDiff;

	//ASTDiff::gitDiffParsingData_.astNodes = new QHash<int, ASTNode*>();

	qDebug() << "Parsing Git Diff";
	ASTDiff::parseGitDiff();

	qDebug() << "Detecting Fundamental Operations";
	ASTDiff::detectFundamentalOperations();

	qDebug() << "Refining Classification";
	ASTDiff::refineClassification();
}

void ASTDiff::print()
{
	QList<ASTNodeDiff*> nodeDiffs = ASTDiff::nodeDiffHashTable_->values();
	QList<ASTNodeDiff*>::iterator nodeDiffIter;
	for (nodeDiffIter = nodeDiffs.begin(); nodeDiffIter != nodeDiffs.end(); nodeDiffIter++)
	{
		(*nodeDiffIter)->print();
	}
}


void ASTDiff::clear()
{
	ASTDiff::gitDiff_ = nullptr;

	ASTDiff::nodeDiffHashTable_->clear();

	ASTDiff::modifiedNodes_->clear();

	ASTDiff::gitDiffParsingData_.astNodes->clear();
}

void ASTDiff::parseGitDiff()
{
	git_diff_foreach(ASTDiff::gitDiff_, gitDiffFileParsingCB, NULL, gitDiffLineParsingCB, &(ASTDiff::gitDiffParsingData_));
}

void ASTDiff::detectFundamentalOperations()
{
	QHash<int, ASTNode*>* nodes = ASTDiff::gitDiffParsingData_.astNodes;

	QList<int> nodeIDs = nodes->uniqueKeys();
	QList<int>::iterator idIter;
	QHash<int, ASTNode*>::iterator nodeIter;

	ASTNode* node;
	ASTNodeDiff* nodeDiff;

	for (idIter = nodeIDs.begin(); idIter != nodeIDs.end(); idIter++)
	{
		switch(nodes->count(*idIter))
		{
			case 1:
				nodeIter = nodes->find(*idIter);
				node = nodeIter.value();
				switch(node->getDiffLineType())
				{
					case AddedLine:
						nodeDiff = new ASTNodeDiff(*idIter);
						nodeDiff->setNodeChangeType(AddedNode);
						nodeDiff->setNewNode(node);

						ASTDiff::nodeDiffHashTable_->insert(*idIter, nodeDiff);
						break;
					case DeletedLine:
						nodeDiff = new ASTNodeDiff(*idIter);
						nodeDiff->setNodeChangeType(DeletedNode);
						nodeDiff->setOldNode(node);

						ASTDiff::nodeDiffHashTable_->insert(*idIter, nodeDiff);
						break;
					default:
						// TODO
						break;
				}
				break;
			case 2:
				nodeDiff = new ASTNodeDiff(*idIter);
				nodeDiff->setNodeChangeType(ModifiedNode);

				nodeIter = nodes->find(*idIter);
				node = nodeIter.value();

				if (node->getDiffLineType() == AddedLine)
				{
					nodeDiff->setNewNode(node);
				}
				else
				{
					nodeDiff->setOldNode(node);
				}

				ASTDiff::modifiedNodes_->insertMulti(node->getID(), node);

				nodeIter++;
				node = nodeIter.value();

				if (node->getDiffLineType() == AddedLine)
				{
					nodeDiff->setNewNode(node);
				}
				else
				{
					nodeDiff->setOldNode(node);
				}
				ASTDiff::modifiedNodes_->insertMulti(node->getID(), node);
				ASTDiff::nodeDiffHashTable_->insert(*idIter, nodeDiff);
				break;
			default:
				// TODO default
				break;
		}
	}
}


void ASTDiff::refineClassification()
{

	QList<int> nodeIDs = ASTDiff::modifiedNodes_->uniqueKeys();
	QList<int>::iterator idIter;
	QHash<int, ASTNode*>::iterator nodeIter;
	QHash<int, ASTNodeDiff*>::iterator nodeDiffIter;

	ASTNode* nodeA;
	ASTNode* nodeB;

	ASTNodeDiff* nodeDiff;
	for (idIter = nodeIDs.begin(); idIter != nodeIDs.end(); idIter++)
	{
		nodeIter = ASTDiff::modifiedNodes_->find(*idIter);
		nodeA = nodeIter.value();
		nodeIter++;
		nodeB = nodeIter.value();

		nodeDiffIter = ASTDiff::nodeDiffHashTable_->find(*idIter);
		nodeDiff = nodeDiffIter.value();

		// check for same name -> reordering detection
		int reorder = QString::compare(nodeA->getName(), nodeB->getName());
		if (reorder != 0) {
			nodeDiff->setReordered(true);
		}
		else {
			nodeDiff->setReordered(false);
		}

		// check for same type -> type change
		int typeChange = QString::compare(nodeA->getType(), nodeB->getType());
		if (typeChange != 0) {
			nodeDiff->setTypeChanged(true);
		}
		else {
			nodeDiff->setTypeChanged(false);
		}

		// check for same value -> update
		int valueUpdate = QString::compare(nodeA->getValue(), nodeB->getValue());
		if (valueUpdate != 0) {
			nodeDiff->setValueUpdated(true);
		}
		else {
			nodeDiff->setValueUpdated(false);
		}
	}


	// move detection
	ASTDiff::moveDetection();







}

void ASTDiff::moveDetection()
{
	// TODO
}
