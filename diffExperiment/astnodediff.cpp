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

#include "astnodediff.h"

#include <QDebug>

ASTNodeDiff::ASTNodeDiff(int id)
{
	ASTNodeDiff::id_ = id;

	ASTNodeDiff::nodeChangeType_ = ModifiedNode;
	ASTNodeDiff::nodeLocationChangeType_ = UnchangedLocation;
	ASTNodeDiff::reordered_ = false;
	ASTNodeDiff::valueUpdated_ = false;
	ASTNodeDiff::typeChanged_ = false;

	ASTNodeDiff::newNode_ = nullptr;
	ASTNodeDiff::oldNode_ = nullptr;
}

ASTNodeDiff::~ASTNodeDiff()
{
	// TODO
}

void ASTNodeDiff::print()
{
	switch (ASTNodeDiff::nodeChangeType_)
	{
		case AddedNode:
			qDebug() << ASTNodeDiff::id_ << "ADD";
			break;

		case DeletedNode:
			qDebug() << ASTNodeDiff::id_ << "DEL";
			break;

		case ModifiedNode:
			qDebug() << ASTNodeDiff::id_ << "MOD";
			if (ASTNodeDiff::nodeLocationChangeType_ == MovedLocation)
			{
				qDebug() << "\t" << "MOV";
			}
			else if (ASTNodeDiff::nodeLocationChangeType_ == UnchangedLocation)
			{
				qDebug() << "\t" << "NO MOV";
			}
			else {
				qDebug() << "\t" << "ERROR!!!";
			}

				qDebug() << "\t" << "Reordered:" << ASTNodeDiff::reordered_ <<"|" << "Updated Value:" << ASTNodeDiff::valueUpdated_ << "|" << "Type Changed:" << ASTNodeDiff::typeChanged_;
			break;

		default:
			qDebug() << ASTNodeDiff::id_ << "ERROR!!!";
	}
}
