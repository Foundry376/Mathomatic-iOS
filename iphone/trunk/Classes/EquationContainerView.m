//
//  EquationContainerView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationContainerView.h"
#import "EquationDivisionView.h"
#import "EquationExponentView.h"

@implementation EquationContainerView

@synthesize innerEquations;
@synthesize parent;

#define kParenthesisPadding 3

- (id)initWithParent:(EquationContainerView*)p
{
    if (self = [super initWithFrame:CGRectZero]) {
        // Initialization code
        
        self.innerEquations = [NSMutableArray array];
        self.backgroundColor = [UIColor clearColor];
        self.userInteractionEnabled = NO; 
        self.clipsToBounds = YES;
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
            float s = [[UIScreen mainScreen] scale];
            self.contentScaleFactor = s;
        }
        
        innerEquationsMax = -1;
        self.parent = p;
    }
    return self;
}

- (void)setShowOpenParenthesis:(BOOL)o
{
    openParen = o;
}

- (void)setShowCloseParenthesis:(BOOL)c
{
    closeParen = c;
}

- (EquationAbstractView*)popInnerEquation
{
    EquationAbstractView * view = [[[self.innerEquations lastObject] retain] autorelease];
    [self.innerEquations removeObjectIdenticalTo: view];
    [view removeFromSuperview];
    return view;
}

- (BOOL)addInnerEquation:(EquationAbstractView*)equation
{
    if ((innerEquationsMax == -1) || ([self.innerEquations count] < innerEquationsMax)){
        [self.innerEquations addObject: equation];
        [self addSubview: equation];
    }else{
        NSLog(@"Attempt to attach %@ to full container", [equation description]);
        return NO;
    }
    return YES;
}

- (BOOL)innerEquationIsFull
{
    if ([self.innerEquations count] < innerEquationsMax)
        return NO;
    else
        return YES;
}

- (EquationAbstractView *)innerEquationAtOffset:(int)offset from:(EquationAbstractView*)v
{
    int ii = [innerEquations indexOfObject: v];
    if ((ii+offset < [innerEquations count]) && (ii+offset >= 0))
        return [innerEquations objectAtIndex: ii + offset];
    else
        return nil;
}

- (void)finalizeEquationTree:(BOOL)clean
{
    [self finalizeEquationTreeForClass: [EquationContainerView class] cleanParenthesis:clean];
    [self finalizeEquationTreeForClass: [EquationExponentView class] cleanParenthesis:clean];

    if ([self finalizeEquationTreeForClass: [EquationDivisionView class] cleanParenthesis: clean])
        alignmentMethod = kAlignmentMethodCentered;
    else if ([innerEquations count] == 1)
        alignmentMethod = [[innerEquations lastObject] alignmentMethod];
    else
        alignmentMethod = kAlignmentMethodBottom;
        
    if (([innerEquations count] < 2) && (clean)){
        [self setShowOpenParenthesis: NO];
        [self setShowCloseParenthesis: NO];
    }
}

- (BOOL)finalizeEquationTreeForClass:(Class)c cleanParenthesis:(BOOL)clean
{
    BOOL found = NO;
    int ii = [innerEquations count] - 1;

    while (ii >= 0){
        EquationAbstractView * v = [innerEquations objectAtIndex: ii];
        if ([v isMemberOfClass: c]){
            [v finalizeEquationTree: clean];
            ii = [innerEquations indexOfObject: v];
            
            found = YES;
        }
        ii--;
    }
    return found;
}

- (void)finalizeTextSize:(int)parentTextSize
{
    for (EquationAbstractView * v in self.innerEquations)
        [v finalizeTextSize: parentTextSize];
    
    minHeight = parentTextSize;
    //minHeight = parentTextSize+kParenthesisPadding*2;
}

- (void)finalizeFrame
{
    [self.innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
    
    int heightAboveBaseline = 0;
    int heightBelowBaseline = 0;
    int x = 0;
    
    for (EquationAbstractView * v in self.innerEquations){
        if (v.alignmentMethod == kAlignmentMethodBottom){
            heightAboveBaseline = fmax([v frame].origin.y + [v frame].size.height, heightAboveBaseline);
        } else if (v.alignmentMethod == kAlignmentMethodCentered){
            heightAboveBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightAboveBaseline);
            heightBelowBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightBelowBaseline);
        }
    }
    
    
    if ([innerEquations count] == 0)
        heightAboveBaseline = minHeight;
     
    CGSize pSize = CGSizeMake(8, 10);
    float pPadding = kParenthesisPadding;
    
    int height = fmax(heightAboveBaseline + heightBelowBaseline, minHeight);
    int parenthesis_width = fmin((height-kParenthesisPadding*2)/20.0, 1)*pSize.width;
    
    if (openParen)
        x = parenthesis_width;
    else
        x = 0;
        
    for (EquationAbstractView * v in self.innerEquations){
        int offset;
        if (v.alignmentMethod == kAlignmentMethodBottom)
            offset = (heightBelowBaseline)/2 + heightAboveBaseline-[v frame].size.height;
        else
            offset = heightAboveBaseline - ([v frame].size.height) / 2;
        [v setFrame: CGRectMake(x, offset, [v frame].size.width, [v frame].size.height)];
        x += [v frame].size.width;
    }
    
    if (closeParen)
        [self setFrame: CGRectMake(0,0, x + parenthesis_width, height)];
    else
        [self setFrame: CGRectMake(0,0, x, height)];
}

- (BOOL)isTreeLegal
{
    for (EquationAbstractView * view in self.innerEquations){
        if (![view isTreeLegal])
            return NO;
    }
    return YES;
}

- (void)drawRect:(CGRect)rect
{
    CGSize size = rect.size;
    CGContextRef c = UIGraphicsGetCurrentContext();
    CGSize pSize = CGSizeMake(8, 10);
    float pPadding = kParenthesisPadding;
    float pWidth = 2;
    
    
    float scale = fmin((size.height-pPadding*2)/20.0, 1);
    if (openParen){
        CGContextDrawImage(c, CGRectMake(0,pPadding,pSize.width*scale,pSize.height*scale), [[UIImage imageNamed: @"parenthesis_bl.png"] CGImage]);
        if (scale == 1)
             CGContextFillRect(c, CGRectMake((pSize.width - pWidth)/2-1, pSize.height+pPadding, pWidth, size.height-pSize.height*2-pPadding*2));
        CGContextDrawImage(c, CGRectMake(0,size.height-pSize.height*scale-pPadding,pSize.width*scale,pSize.height*scale), [[UIImage imageNamed: @"parenthesis_tl.png"] CGImage]);
    }
    if (closeParen){
        CGContextDrawImage(c, CGRectMake(size.width-pSize.width*scale,pPadding,pSize.width*scale,pSize.height*scale), [[UIImage imageNamed: @"parenthesis_br.png"] CGImage]);
        if (scale == 1)
            CGContextFillRect(c, CGRectMake(size.width-(pSize.width - pWidth)/2, pSize.height+pPadding, pWidth, size.height-pSize.height*2-pPadding*2));
        CGContextDrawImage(c, CGRectMake(size.width-pSize.width*scale,size.height-pSize.height*scale-pPadding,pSize.width*scale,pSize.height
        *scale), [[UIImage imageNamed: @"parenthesis_tr.png"] CGImage]);    
    }
}

- (NSString*)description
{
    NSMutableString * description = [NSMutableString stringWithString: @"("];
    for (EquationAbstractView * view in self.innerEquations)
        [description appendString: [view description]];
    [description appendFormat: @")"];
    return description;
}

- (void)unlink
{
    for (EquationAbstractView * view in self.innerEquations)
        [view unlink];
    [parent release];
    parent = nil;
}

- (void)dealloc {
    [innerEquations release];
    [super dealloc];
}


@end
