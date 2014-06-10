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

#ifndef ASTNODEDIFF_H
#define ASTNODEDIFF_H

#include <astnode.h>

typedef enum {AddedNode, DeletedNode, ModifiedNode} nodeChangeType;

typedef enum {MovedLocation, UnchangedLocation} nodeLocationChangeType;

class ASTNodeDiff
{
	public:
		ASTNodeDiff(int id);
		~ASTNodeDiff();

		void setNodeChangeType(nodeChangeType changeType);

		void setReordered(bool isReordered);
		void setTypeChanged(bool typeChanged);
		void setValueUpdated(bool valueUpdated);

		void setNewNode(ASTNode* newNode);
		void setOldNode(ASTNode* oldNode);

		void print();

	private:
		int id_;

		nodeChangeType nodeChangeType_;
		nodeLocationChangeType nodeLocationChangeType_;
		bool reordered_;
		bool valueUpdated_;
		bool typeChanged_;

		ASTNode* newNode_;
		ASTNode* oldNode_;
};

inline void ASTNodeDiff::setNodeChangeType(nodeChangeType changeType) {ASTNodeDiff::nodeChangeType_ = changeType;}

inline void ASTNodeDiff::setReordered(bool isReordered) {ASTNodeDiff::reordered_ = isReordered;}
inline void ASTNodeDiff::setTypeChanged(bool typeChanged) {ASTNodeDiff::typeChanged_ = typeChanged;}
inline void ASTNodeDiff::setValueUpdated(bool valueUpdated) {ASTNodeDiff::valueUpdated_ = valueUpdated;}

inline void ASTNodeDiff::setNewNode(ASTNode* newNode) {ASTNodeDiff::newNode_ = newNode;}
inline void ASTNodeDiff::setOldNode(ASTNode* oldNode) {ASTNodeDiff::oldNode_ = oldNode;}

#endif // ASTNODEDIFF_H
