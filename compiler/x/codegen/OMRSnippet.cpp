/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

#include "codegen/Snippet.hpp"

#include <stddef.h>                                // for NULL
#include <stdint.h>                                // for int32_t, uint8_t
#include "codegen/CodeGenerator.hpp"               // for CodeGenerator
#include "codegen/ConstantDataSnippet.hpp"
#include "infra/Assert.hpp"                        // for TR_ASSERT
#include "ras/Debug.hpp"                           // for TR_Debug
#include "x/codegen/DataSnippet.hpp"
#include "x/codegen/HelperCallSnippet.hpp"
#include "x/codegen/RestartSnippet.hpp"
#include "x/codegen/X86Ops.hpp"                    // for ::JMP4, etc
#include "codegen/UnresolvedDataSnippet.hpp"

namespace TR { class AMD64FPConversionSnippet; }
namespace TR { class X86BoundCheckWithSpineCheckSnippet; }
namespace TR { class X86CallSnippet; }
namespace TR { class X86CheckFailureSnippet; }
namespace TR { class X86CheckFailureSnippetWithResolve; }
namespace TR { class X86DivideCheckSnippet; }
namespace TR { class X86FPConvertToIntSnippet; }
namespace TR { class X86FPConvertToLongSnippet; }
namespace TR { class X86ForceRecompilationSnippet; }
namespace TR { class X86GuardedDevirtualSnippet; }
namespace TR { class X86PicDataSnippet; }
namespace TR { class X86RecompilationSnippet; }
namespace TR { class X86ScratchArgHelperCallSnippet; }
namespace TR { class X86SpineCheckSnippet; }
namespace TR { class X86UnresolvedVirtualCallSnippet; }
namespace TR { class X86fbits2iSnippet; }
namespace TR { class LabelSymbol; }
namespace TR { class Node; }

#ifdef J9_PROJECT_SPECIFIC
#include "x/codegen/JNIPauseSnippet.hpp"
#include "x/codegen/PassJNINullSnippet.hpp"
#include "x/codegen/WriteBarrierSnippet.hpp"
#endif

OMR::X86::Snippet::Snippet(
      TR::CodeGenerator *cg,
      TR::Node *node,
      TR::LabelSymbol *label,
      bool isGCSafePoint) :
   OMR::Snippet(cg, node, label, isGCSafePoint)
   {
   }


OMR::X86::Snippet::Snippet(
      TR::CodeGenerator *cg,
      TR::Node *node,
      TR::LabelSymbol *label) :
   OMR::Snippet(cg, node, label)
   {
   }


const char *
TR_Debug::getNamex(TR::Snippet *snippet)
   {
   switch (snippet->getKind())
      {
#ifdef J9_PROJECT_SPECIFIC
      case TR::Snippet::IsCall:
         return "Call Snippet";
         break;
      case TR::Snippet::IsVPicData:
         return "VPic Data";
         break;
      case TR::Snippet::IsIPicData:
         return "IPic Data";
         break;
      case TR::Snippet::IsUnresolvedVirtualCall:
         return "Unresolved Virtual Call Snippet";
         break;
      case TR::Snippet::IsWriteBarrier:
      case TR::Snippet::IsWriteBarrierAMD64:
         return "Write Barrier Snippet";
         break;
      case TR::Snippet::IsJNIPause:
         return "JNI Pause Snippet";
         break;
      case TR::Snippet::IsScratchArgHelperCall:
         return "Helper Call Snippet with scratch-reg argument";
         break;
      case TR::Snippet::IsForceRecompilation:
         return "Force Recompilation Snippet";
         break;
      case TR::Snippet::IsRecompilation:
         return "Recompilation Snippet";
         break;
#endif
      case TR::Snippet::IsCheckFailure:
         return "Check Failure Snippet";
         break;
      case TR::Snippet::IsCheckFailureWithResolve:
         return "Check Failure Snippet with Resolve Call";
         break;
      case TR::Snippet::IsBoundCheckWithSpineCheck:
         return "Bound Check with Spine Check Snippet";
         break;
      case TR::Snippet::IsSpineCheck:
         return "Spine Check Snippet";
         break;
      case TR::Snippet::IsConstantData:
         return "Constant Data Snippet";
         break;
      case TR::Snippet::IsData:
         return "Data Snippet";
      case TR::Snippet::IsDivideCheck:
         return "Divide Check Snippet";
         break;

#ifdef J9_PROJECT_SPECIFIC
      case TR::Snippet::IsGuardedDevirtual:
         return "Guarded Devirtual Snippet";
         break;
#endif
      case TR::Snippet::IsHelperCall:
         return "Helper Call Snippet";
         break;
      case TR::Snippet::IsFPConversion:
         return "FP Conversion Snippet";
         break;
      case TR::Snippet::IsFPConvertToInt:
         return "FP Convert To Int Snippet";
         break;
      case TR::Snippet::IsFPConvertToLong:
         return "FP Convert To Long Snippet";
         break;
      case TR::Snippet::IsFPConvertAMD64:
         return "FP Convert To Int/Long Snippet";
         break;
      case TR::Snippet::Isfbits2i:
         return "fbits2i Snippet";
         break;
      case TR::Snippet::IsPassJNINull:
         return "Pass JNI Null Snippet";
         break;
      case TR::Snippet::IsUnresolvedDataIA32:
      case TR::Snippet::IsUnresolvedDataAMD64:
         return "Unresolved Data Snippet";
         break;
      case TR::Snippet::IsRestart:
      default:
         TR_ASSERT(0, "unexpected snippet kind: %d", snippet->getKind());
         return "Unknown snippet kind";
      }
   }

void
TR_Debug::printx(TR::FILE *pOutFile, TR::Snippet *snippet)
   {
   // TODO:AMD64: Clean up these #ifdefs
   if (pOutFile == NULL)
      return;
   switch (snippet->getKind())
      {
#ifdef J9_PROJECT_SPECIFIC
      case TR::Snippet::IsCall:
         print(pOutFile, (TR::X86CallSnippet *)snippet);
         break;
      case TR::Snippet::IsIPicData:
      case TR::Snippet::IsVPicData:
         print(pOutFile, (TR::X86PicDataSnippet *)snippet);
         break;
      case TR::Snippet::IsUnresolvedVirtualCall:
         print(pOutFile, (TR::X86UnresolvedVirtualCallSnippet *)snippet);
         break;
      case TR::Snippet::IsWriteBarrier:
         print(pOutFile, (TR::IA32WriteBarrierSnippet *)snippet);
         break;
#ifdef TR_TARGET_64BIT
      case TR::Snippet::IsWriteBarrierAMD64:
         print(pOutFile, (TR::AMD64WriteBarrierSnippet *)snippet);
         break;
#endif
      case TR::Snippet::IsJNIPause:
         print(pOutFile, (TR::X86JNIPauseSnippet  *)snippet);
         break;
      case TR::Snippet::IsPassJNINull:
         print(pOutFile, (TR::X86PassJNINullSnippet  *)snippet);
         break;
      case TR::Snippet::IsCheckFailure:
         print(pOutFile, (TR::X86CheckFailureSnippet *)snippet);
         break;
      case TR::Snippet::IsCheckFailureWithResolve:
         print(pOutFile, (TR::X86CheckFailureSnippetWithResolve *)snippet);
         break;
      case TR::Snippet::IsBoundCheckWithSpineCheck:
         print(pOutFile, (TR::X86BoundCheckWithSpineCheckSnippet *)snippet);
         break;
      case TR::Snippet::IsSpineCheck:
         print(pOutFile, (TR::X86SpineCheckSnippet *)snippet);
         break;
      case TR::Snippet::IsScratchArgHelperCall:
         print(pOutFile, (TR::X86ScratchArgHelperCallSnippet *)snippet);
         break;
      case TR::Snippet::IsForceRecompilation:
         print(pOutFile, (TR::X86ForceRecompilationSnippet  *)snippet);
         break;
      case TR::Snippet::IsRecompilation:
         print(pOutFile, (TR::X86RecompilationSnippet *)snippet);
         break;
#endif
      case TR::Snippet::IsConstantData:
         print(pOutFile, (TR::IA32ConstantDataSnippet *)snippet);
         break;
      case TR::Snippet::IsData:
         print(pOutFile, (TR::IA32DataSnippet *)snippet);
         break;
      case TR::Snippet::IsDivideCheck:
         print(pOutFile, (TR::X86DivideCheckSnippet  *)snippet);
         break;

#ifdef J9_PROJECT_SPECIFIC
      case TR::Snippet::IsGuardedDevirtual:
         print(pOutFile, (TR::X86GuardedDevirtualSnippet  *)snippet);
         break;
#endif
      case TR::Snippet::IsHelperCall:
         print(pOutFile, (TR::X86HelperCallSnippet  *)snippet);
         break;
      case TR::Snippet::IsFPConvertToInt:
         print(pOutFile, (TR::X86FPConvertToIntSnippet  *)snippet);
         break;
      case TR::Snippet::IsFPConvertToLong:
         print(pOutFile, (TR::X86FPConvertToLongSnippet  *)snippet);
         break;
#ifdef TR_TARGET_64BIT
      case TR::Snippet::IsFPConvertAMD64:
         print(pOutFile, (TR::AMD64FPConversionSnippet *)snippet);
         break;
#endif
      case TR::Snippet::Isfbits2i:
         print(pOutFile, (TR::X86fbits2iSnippet  *)snippet);
         break;
      case TR::Snippet::IsUnresolvedDataIA32:
         print(pOutFile, (TR::UnresolvedDataSnippet *)snippet);
         break;
#ifdef TR_TARGET_64BIT
      case TR::Snippet::IsUnresolvedDataAMD64:
         print(pOutFile, (TR::UnresolvedDataSnippet *)snippet);
         break;
#endif
      case TR::Snippet::IsRestart:
      default:
         TR_ASSERT(0, "unexpected snippet kind: %d", snippet->getKind());
      }
   }

int32_t
TR_Debug::printRestartJump(TR::FILE *pOutFile, TR::X86RestartSnippet * snippet, uint8_t *bufferPos)
   {
   int32_t size = snippet->estimateRestartJumpLength(JMP4, bufferPos - (uint8_t*)snippet->cg()->getBinaryBufferStart());
   printPrefix(pOutFile, NULL, bufferPos, size);
   printLabelInstruction(pOutFile, "jmp", snippet->getRestartLabel());
   return size;
   }

#ifdef J9_PROJECT_SPECIFIC
int32_t
TR_Debug::printRestartJump(TR::FILE *pOutFile, TR::AMD64WriteBarrierSnippet * snippet, uint8_t *bufferPos)
   {
   int32_t size = snippet->estimateRestartJumpLength(JMP4, bufferPos - (uint8_t*)snippet->cg()->getBinaryBufferStart());
   printPrefix(pOutFile, NULL, bufferPos, size);
   printLabelInstruction(pOutFile, "jmp", snippet->getRestartLabel());
   return size;
   }

#endif


int32_t
TR_Debug::printRestartJump(TR::FILE *pOutFile, TR::X86RestartSnippet * snippet, uint8_t *bufferPos, int32_t branchOp, const char *branchOpName)
   {
   int32_t size = snippet->estimateRestartJumpLength((TR_X86OpCodes) branchOp, bufferPos - (uint8_t*)snippet->cg()->getBinaryBufferStart());
   printPrefix(pOutFile, NULL, bufferPos, size);
   printLabelInstruction(pOutFile, branchOpName, snippet->getRestartLabel());
   return size;
   }
